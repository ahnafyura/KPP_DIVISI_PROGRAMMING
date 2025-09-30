#include <bits/stdc++.h>
using namespace std;

const int energiMaks = 1000;
const bool pakeFaktorWaktu = false;

class Jalan {
public:
    int tujuan;
    int panjang;
    int halangan;
    Jalan(int t=0,int p=0,int h=0):tujuan(t),panjang(p),halangan(h){}
};

class Kondisi {
public:
    int simpul;
    int menitMod60;
    int jamGanjil;
    int terpakai;

    Kondisi(int s=0,int m=0,int j=0,int t=0):simpul(s),menitMod60(m),jamGanjil(j),terpakai(t){}
};

class Peta {
public:
    int N;
    unordered_map<string,int> namaKeId;
    vector<string> idKeNama;
    vector<vector<Jalan>> tetangga;
    vector<char> titikIstirahat;
    vector<char> titikCas;

    Peta(int n=0){ init(n); }

    void init(int n){
        N = max(0, n);
        namaKeId.clear();
        idKeNama.clear();
        tetangga.clear();
        titikIstirahat.clear();
        titikCas.clear();
    }

    int ambilId(const string &nama){
        auto it = namaKeId.find(nama);
        if(it != namaKeId.end()) return it->second;
        int id = (int)idKeNama.size();
        namaKeId[nama] = id;
        idKeNama.push_back(nama);
        tetangga.emplace_back();
        titikIstirahat.push_back(0);
        titikCas.push_back(0);
        return id;
    }

    void tambahJalan(const string &u, const string &v, int w, int o){
        int a = ambilId(u);
        int b = ambilId(v);
        tetangga[a].push_back(Jalan(b,w,o));
        tetangga[b].push_back(Jalan(a,w,o));
    }
};

class Robot {
public:
    Peta &peta;
    int jamMulai;

    Robot(Peta &p, int jam): peta(p), jamMulai(jam) {}
    string buatKunci(const Kondisi &k){
        return to_string(k.simpul) + "_" + to_string(k.menitMod60) + "_" + to_string(k.jamGanjil) + "_" + to_string(k.terpakai);
    }

    bool cariDanCetak(const string &namaAwal, const string &namaTujuan){
        if(peta.namaKeId.find(namaAwal) == peta.namaKeId.end() ||
           peta.namaKeId.find(namaTujuan) == peta.namaKeId.end()){
            cout << "Robot gagal dalam mencapai tujuan :(\n";
            return false;
        }

        int idAwal = peta.ambilId(namaAwal);
        int idTujuan = peta.ambilId(namaTujuan);

        using Item = tuple<int,int,Kondisi>;
        struct Banding {
            bool operator()(Item const &a, Item const &b) const {
                if(get<0>(a) != get<0>(b)) return get<0>(a) > get<0>(b);
                return get<1>(a) > get<1>(b);
            }
        };

        priority_queue<Item, vector<Item>, Banding> pq;
        unordered_map<string,int> terbaik;
        unordered_map<string, pair<string,int>> parent;

        Kondisi awal(idAwal, 0, jamMulai % 2, 0);
        string keyAwal = buatKunci(awal);
        terbaik[keyAwal] = 0;
        parent[keyAwal] = make_pair(string("ROOT"), 0);
        pq.push({0, 0, awal});

        bool ketemu = false;
        string keyAkhir;
        Kondisi kondisiAkhir;

        while(!pq.empty()){
            auto [energiTotal, menitTotal, state] = pq.top(); pq.pop();
            string keyState = buatKunci(state);
            if(terbaik.find(keyState) != terbaik.end() && terbaik[keyState] < energiTotal) continue;

            if(state.simpul == idTujuan){
                ketemu = true;
                keyAkhir = keyState;
                kondisiAkhir = state;
                break;
            }

            if(peta.titikCas[state.simpul] && state.terpakai != 0){
                Kondisi ns = state; ns.terpakai = 0;
                string k2 = buatKunci(ns);
                if(!terbaik.count(k2) || terbaik[k2] > energiTotal){
                    terbaik[k2] = energiTotal;
                    parent[k2] = make_pair(keyState, menitTotal);
                    pq.push({energiTotal, menitTotal, ns});
                }
            }

            if(peta.titikIstirahat[state.simpul]){
                int tunggu = (60 - state.menitMod60) % 60;
                if(tunggu == 0) tunggu = 60;
                int menitBaru = menitTotal + tunggu;
                int modBaru = (state.menitMod60 + tunggu) % 60;
                int parBaru = 1 - state.jamGanjil;
                Kondisi ns(state.simpul, modBaru, parBaru, state.terpakai);
                string k2 = buatKunci(ns);
                if(!terbaik.count(k2) || terbaik[k2] > energiTotal){
                    terbaik[k2] = energiTotal;
                    parent[k2] = make_pair(keyState, menitBaru);
                    pq.push({energiTotal, menitBaru, ns});
                }
            }

            // traverse edges
            for(const Jalan &e : peta.tetangga[state.simpul]){
                int dasar = e.panjang + e.halangan;
                if(dasar < 0) continue;
                double faktor = 1.0;
                if(pakeFaktorWaktu){
                    faktor = (state.jamGanjil == 1) ? 1.3 : 0.8;
                }
                int biaya = (int)ceil(dasar * faktor);
                if(biaya > energiMaks) continue;
                if(state.terpakai + biaya > energiMaks) continue;

                int menitBaru = menitTotal + 2;
                int menitAbsolut = jamMulai * 60 + menitBaru;
                int parBaru = (menitAbsolut / 60) % 2;
                int modBaru = menitBaru % 60;
                int terpakaiBaru = state.terpakai + biaya;
                if(peta.titikCas[e.tujuan]) terpakaiBaru = 0;

                int energiBaru = energiTotal + biaya;
                Kondisi ns(e.tujuan, modBaru, parBaru, terpakaiBaru);
                string k2 = buatKunci(ns);

                if(!terbaik.count(k2) || terbaik[k2] > energiBaru){
                    terbaik[k2] = energiBaru;
                    parent[k2] = make_pair(keyState, menitBaru);
                    pq.push({energiBaru, menitBaru, ns});
                }
            }
        }

        if(!ketemu){
            cout << "Robot gagal dalam mencapai tujuan :(\n";
            return false;
        }

        vector<pair<int,int>> urut;
        string curKey = keyAkhir;
        while(true){
            auto it = parent.find(curKey);
            if(it == parent.end()) break;
            string parentKey = it->second.first;
            int arrival = it->second.second;
            int pos1 = curKey.find('_');
            string nodeStr = curKey.substr(0, pos1);
            int nodeId = stoi(nodeStr);
            urut.push_back({nodeId, arrival});
            if(parentKey == "ROOT") break;
            curKey = parentKey;
        }
        reverse(urut.begin(), urut.end());

        if(urut.size()>0 && urut.front().second != 0){
            urut.insert(urut.begin(), {idAwal, 0});
        }

        cout << "Total energi minimum: " << terbaik[keyAkhir] << "\n";

        cout << "Jalur: ";
        for(size_t i=0;i<urut.size();++i){
            if(i) cout << " -> ";
            cout << peta.idKeNama[urut[i].first];
        }
        cout << "\nWaktu tiba:\n";
        for(size_t i=0;i<urut.size();++i){
            cout << peta.idKeNama[urut[i].first] << " (menit " << urut[i].second << ")\n";
        }

        return true;
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    if(!(cin >> N >> M)) return 0;

    Peta p;
    p.init(N);

    for(int i=0;i<M;i++){
        string u, v;
        int w, o;
        cin >> u >> v >> w >> o;
        p.tambahJalan(u, v, w, o);
    }

    string S, T;
    cin >> S >> T;

    auto bacaDaftar = [&](vector<char> &penanda){
        string baris;
        cin >> ws;
        getline(cin, baris);
        if(baris.size() == 0){
            if((int)penanda.size() < (int)p.idKeNama.size()) penanda.resize(p.idKeNama.size());
            return;
        }
        stringstream ss(baris);
        string token;
        vector<int> ids;
        while(ss >> token){
            if(token == "-"){
                penanda.assign(p.idKeNama.size(), 0);
                return;
            }
            int id = p.ambilId(token);
            ids.push_back(id);
        }
        if((int)penanda.size() < (int)p.idKeNama.size()) penanda.resize(p.idKeNama.size());
        for(int id: ids) penanda[id] = 1;
    };

    bacaDaftar(p.titikIstirahat);
    bacaDaftar(p.titikCas);
    vector<char> dummy;
    bacaDaftar(dummy);
    bacaDaftar(dummy);

    int jamAwal;
    cin >> jamAwal;

    Robot bot(p, jamAwal);
    bot.cariDanCetak(S, T);

    return 0;
}
