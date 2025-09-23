# KPP Programming Pelatdas 2025 (Robot Navigator)

## Deskripsi

Program ini merupakan solusi dari soal KPP Programming Pelatdas 2025 yang menguji kemampuan penerapan OOP (Object-Oriented Programming) dan algoritma path finding dalam simulasi robot otonom. Robot harus menavigasi jaringan jalan dari node **S** ke **T** dengan mempertimbangkan panjang jalan, obstacle, kondisi waktu (ganjil/genap), kemampuan menunggu di rest point, serta mengisi energi di charging station.

Tujuan utama adalah mencari jalur yang meminimalkan **total energi yang dikonsumsi**, serta menampilkan lintasan dan waktu tiba di setiap node.

---

## Aturan Penting

1. **Graf**

   * Jumlah node = `N`, jumlah edge = `M`.
   * Setiap edge memiliki dua atribut: panjang dasar `w` dan obstacle `o`.
   * Total energi dasar untuk melewati edge = `w + o`.

2. **Pengaruh waktu (jam ganjil/genap)**

   * Jika melewati edge pada **jam ganjil** → energi bertambah **30%** (× 1.3).
   * Jika melewati edge pada **jam genap** → energi berkurang **20%** (× 0.8).

3. **Rest Point (R)**

   * Robot boleh menunggu integer menit di node ini untuk mengubah kondisi waktu (ganjil ke genap).

4. **Charging Station (C)**

   * Robot dapat mengisi ulang energi ke maksimum (1000) saat berada di node ini.

5. **Energi Maksimum**

   * Kapasitas energi = **1000 unit**.
   * Robot memulai perjalanan dengan energi penuh.

6. **Tujuan**

   * Menemukan jalur dari **S** ke **T** dengan **energi minimum**.
   * Output: total energi minimum, jalur, dan waktu tiba di setiap node.
   * Jika gagal → tampilkan `Robot gagal dalam mencapai tujuan :(`.

---

## Format Masukan

```
Baris 1     : N M
Baris 2..M+1: u v w o   (edge tak-berarah)
Baris M+2   : S T
Baris M+3   : daftar node Rest Point (atau -)
Baris M+4   : daftar node Charging Station (atau -)
Baris M+5   : daftar node M (Mechanic) atau -
Baris M+6   : daftar node E (Electrical) atau -
Baris terakhir: Jam mulai perjalanan (integer)
```

---

## Format Keluaran

### Jika berhasil:

```
Total energi minimum: [nilai]
Jalur: S -> ... -> T
Waktu tiba:
S (menit 0)
A (menit 2)
...
```

### Jika gagal:

```
Robot gagal dalam mencapai tujuan :(
```

---

## Contoh Input & Output

### Sample Input 1

```
6 7
S A 200 0
A B 300 50
A C 150 0
B D 400 100
C D 100 0
D E 250 0
E T 200 0
S T
C
E
M
-
1
```

### Sample Output 1

```
Total energi minimum: 960
Jalur: S -> A -> C -> D -> E -> T
Waktu tiba:
S (menit 0)
A (menit 2)
C (menit 4)
D (menit 6)
E (menit 8)
T (menit 10)
```

### Penjelasan Input

* `6 7` → jumlah node = 6, jumlah edge = 7.
* `S A 200 0` → jalan dari **S ke A** sepanjang 200, tanpa tambahan energi (biaya total = 200).
* `A B 300 50` → jalan dari **A ke B** sepanjang 300 + obstacle 50 → biaya total = 350.
* `A C 150 0` → jalan dari **A ke C** sepanjang 150, biaya total = 150.
* `B D 400 100` → jalan dari **B ke D** sepanjang 400 + obstacle 100 → biaya total = 500.
* `C D 100 0` → jalan dari **C ke D** sepanjang 100, biaya total = 100.
* `D E 250 0` → jalan dari **D ke E** sepanjang 250, biaya total = 250.
* `E T 200 0` → jalan dari **E ke T** sepanjang 200, biaya total = 200.
* `S T` → start = S, target = T.
* `C` → node C adalah **charging station**.
* `E` → node E adalah **rest point**.
* `M` → aturan lalu lintas terkait **jam ganjil/genap**.
* `-` → delimiter.
* `1` → waktu awal = 1 (ganjil).

### Perhitungan Jalur

Misalnya dipilih jalur **S → A → C → D → E → T**:

* Energi = 200 (S-A) + 150 (A-C) + 100 (C-D) + 250 (D-E) + 200 (E-T) = **900**.
* Jika kapasitas energi robot < 900, maka wajib mengisi ulang di **C** (charging station).
* Dari sisi waktu: setiap edge menambah 1 satuan waktu. Mulai dari waktu = 1 (ganjil), maka:

  * S→A tiba pada t=2 (genap)
  * A→C tiba pada t=3 (ganjil)
  * C→D tiba pada t=4 (genap)
  * D→E tiba pada t=5 (ganjil)
  * E→T tiba pada t=6 (genap)
  * Jika aturan M menyatakan misalnya jalur C ke D hanya boleh ditempuh pada jam ganjil, maka pada t=3 robot belum bisa jalan langsung ke D, sehingga harus menunggu di C. Dengan menunggu 1 satuan waktu (hingga t=4 genap), sinkronisasi gagal karena jalur butuh ganjil. Jadi meski energi cukup, aturan waktu tetap bisa membuat jalur tidak valid.


### Sample Input 2

```
4 3
S A 600 200
A B 300 0
B T 200 0
S T
-
-
-
-
1
```

### Sample Output 2

```
Robot gagal dalam mencapai tujuan :(
```

**Penjelasan mengapa perjalanan gagal pada Sample Input 2:**

1. **Kapasitas energi awal:** robot memulai dengan energi penuh = **1000**.
2. **Edge pertama (S -> A):** total energi dasar = `w + o = 600 + 200 = 800`.

   * Karena `startHour = 1` (jam ganjil), faktor waktu menambah **30%** → energi sesungguhnya = `800 × 1.3 = 1040`.
   * **1040 > 1000**, sehingga robot **tidak memiliki cukup energi** untuk melewati edge pertama. Karena tidak ada charging station atau rest point sebelum edge itu, robot tidak bisa menempuh langkah pertama.
3. **Catatan tambahan (tanpa pengaruh waktu):** bahkan jika faktor waktu diabaikan, setelah melewati S→A robot tersisa `1000 - 800 = 200` energi, yang masih **tidak cukup** untuk melewati A→B (membutuhkan 300). Jadi ketiadaan charging station pada jalur dan tidak adanya rest point/opsi menunggu membuat rute tidak dapat dilanjutkan.

**Kesimpulan:** kegagalan disebabkan oleh kebutuhan energi yang melebihi kapasitas pada edge pertama akibat obstacle + pengaruh jam ganjil, serta tidak tersedianya charging station atau rest point yang memungkinkan robot menunggu atau mengisi ulang sebelum mencoba melewati edge tersebut.

---

## Implementasi

* **Bahasa**: C++
* **Paradigma**: Object-Oriented Programming (OOP)
* **Algoritma Path Finding**: Modifikasi **Dijkstra/A**\* dengan state (waktu, energi, rest, charging)\*\*

Kelas utama dalam implementasi:

* `Graph` → representasi connected road.
* `RobotNavigator` → logika navigasi, termasuk path finding.
* `State` & `StateHash` → representasi kondisi robot (posisi, waktu, energi).

---

## ▶ Cara Menjalankan

1. Clone repository:

   ```bash
   git clone https://github.com/username/repo-name.git
   cd repo-name
   ```
2. Kompilasi program:

   ```bash
   g++ main.cpp -o robot_navigator
   ```
3. Jalankan program:

   ```bash
   ./robot_navigator
   ```
4. Masukkan input sesuai format soal.

---

---

## Author

Program ini dibuat sebagai solusi tugas akhir **KPP Programming Pelatdas 2025** oleh Ahnaf Al Ghiffarri Ahtasyafi (5024251020) Teknik Komputer
