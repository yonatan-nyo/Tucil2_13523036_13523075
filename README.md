# QuadTree Image Compression

<div align="center">
  <img src="assets/hmif.gif" alt="HMIF">
</div>

## Deskripsi Singkat

QuadTree Image Compression adalah sebuah program berbasis C++ yang mengimplementasikan struktur data **QuadTree** untuk membagi dan mengompresi citra digital secara adaptif. Program ini mengombinasikan efisiensi pemrosesan gambar dengan fleksibilitas analisis kesalahan (error) melalui berbagai metode statistik. Tujuannya adalah untuk melakukan kompresi citra tanpa kehilangan terlalu banyak detail visual, dengan tetap menjaga kontrol penuh terhadap parameter kompresi.

Program ini mendukung berbagai format gambar seperti `.png` , `.jpg` , `.jpeg` , `.bmp`, `.tga`, dan memberikan dua pendekatan kompresi:

- **Kompresi Otomatis** berdasarkan target persentase kompresi,
- **Kompresi Manual** berdasarkan nilai ambang batas (threshold) dari error.

### Fitur Utama:

- Analisis blok citra menggunakan QuadTree.
- Penghitungan error dengan berbagai metode statistik:
  - **Variance (Varian)**
  - **Mean Absolute Deviation (MAD)**
  - **Max Pixel Difference**
  - **Entropy**
  - **SSIM**
- Dua mode kompresi: otomatis dan manual.
- Penyimpanan hasil dalam format gambar dan GIF.
- Kompatibel dengan Unix-based OS dan WSL (untuk pengguna Windows).

## Requirement & Instalasi

Agar program dapat berjalan dengan baik, berikut adalah beberapa requirement yang harus dipenuhi:

### Software yang Dibutuhkan

- Compiler C++ modern (minimal mendukung C++17) — disarankan menggunakan `g++`
- `Make` — untuk mempermudah kompilasi
- `Valgrind` — opsional, untuk debugging kebocoran memori
- WSL (Windows Subsystem for Linux) — **hanya jika Anda menggunakan Windows**

### Cara Instalasi di Ubuntu/WSL:

1. Update daftar paket terlebih dahulu:

   ```sh
   sudo apt-get update
   ```

2. Instal `g++` dan `gdb`:

   ```sh
   sudo apt-get install build-essential gdb
   ```

3. (Opsional) Instal `valgrind` untuk pengecekan memori:
   ```sh
   sudo apt install valgrind
   ```

Pastikan juga direktori kerja Anda sudah memiliki struktur folder yang sesuai, dan file gambar yang ingin diuji tersedia di dalam atau dapat diakses dari program.

## Cara Menjalankan

### Kompilasi Program

Gunakan perintah berikut untuk meng-compile seluruh program menggunakan Makefile yang telah disediakan:

```sh
make all
```

### Jalankan Program dengan Deteksi Memori (Opsional)

Jika Anda ingin mengecek kemungkinan kebocoran memori menggunakan Valgrind:

```sh
make debug
```

### Masukkan Lokasi File

Program akan meminta Anda untuk memasukkan path ke file gambar. Path yang diterima dapat berupa:

- **Path relatif** dari direktori program:
  ```
  test/a/a.png
  test/b/c.png
  test/c/c.jpg
  ```
- **Path absolut**:
  ```
  /home/user/Documents/images/a.png
  ```

## Mode Kompresi

### 1. Kompresi Otomatis Berdasarkan Target Rasio

Jika Anda ingin mengompresi gambar dengan target rasio tertentu, cukup masukkan angka desimal antara `0.0` hingga `1.0`, yang merepresentasikan seberapa besar pengurangan ukuran gambar yang diinginkan.

Contoh:

- `0.3` berarti target kompresi adalah 30% dari ukuran asli.
- `0.0` berarti fitur kompresi otomatis dinonaktifkan.

### 2. Kompresi Manual

Jika Anda ingin kontrol lebih penuh atas proses kompresi, gunakan mode manual:

- Pilih metode error yang diinginkan (Variance, MAD, dll).
- Masukkan **threshold** (nilai ambang batas) yang menentukan sensitivitas pemisahan blok.
- Masukkan ukuran **minimum blok** untuk menentukan resolusi terkecil pembagian.

Dengan mode ini, Anda dapat menyesuaikan tingkat detail akhir sesuai dengan kebutuhan spesifik (misal untuk keperluan pengolahan citra atau visualisasi).

## Output Program

- Hasil kompresi akan disimpan dalam bentuk **gambar statis** dan **GIF animasi** yang menunjukkan proses pembagian blok.
- Anda akan diminta untuk memasukkan **lokasi penyimpanan** hasil output tersebut, baik berupa path absolut maupun relatif.

Contoh path output:

```
test/hasil.png
test/animasi.gif
```

## Contoh Penggunaan

```
test/a/a.png
0
1
10
1


```

enter 2 kali untuk default file output naming

## Contoh Hasil Eksekusi menggunakan WSL

```
nyo@Yonatan-Edward-Njoto:/mnt/c/.../Tucil2_13523036_13523075$ make all
g++ -Wall -Werror -std=c++17   src/main.cpp src/utils/gif/GIFMaker.cpp src/utils/image/File.cpp src/utils/image/Image.cpp src/utils/image/Pixel.cpp src/utils/quadtree/QuadTreeNode.cpp src/utils/quadtree/QuadTreeNodeError.cpp src/utils/quadtree/QuadTreeNodeGIF.cpp -o bin/main
./bin/main
=========================
              ..
             ( '`< ITB
              )(
       ( ----'  '.
       (         ;
        (_______,'
~^~^~^~^~^~^~^~^~^~^~^~^~
QuadTree Image Compressor
=========================

Enter image file name: test/a/a.png
Checking file at: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a.png"
File exists, initializing image.
Image initialized.

Image Info:
Width: 612
Height: 533
Channels: 1
Total Pixels: 326196

Do you want to use percentage compression? (0.0 - 1.0) (default: 0, using threshold and minimum block size): 0

Pilih Metode Perhitungan Error
1. Variance
2. Mean Absolute Deviation
3. Max Pixel Difference
4. Entropy
5. Structural Similarity Index
Masukkan metode (1-5): 1

Masukkan Threshold atau ambang batas ( 0 ≤ x ≤ 65025 ): 10
Masukkan ukuran blok minimum: 1

Masukkan alamat file hasil
[Default: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.png"]
(tekan Enter untuk default):
Tidak ada path diberikan. Menggunakan default: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.png"

Masukkan alamat file hasil GIF
[Default: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.gif"]
(tekan Enter untuk default):
Tidak ada path diberikan. Menggunakan default: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.gif"

Processing image with method 1 and threshold 10
Minimum block size: 1
QuadTreeNode initialized.
Building matrix...
Matrix built.
Saving image...
channels: 1
Image successfully saved to: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.png"
Image saved to: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.png"

====== stats =======
Waktu Eksekusi: 0.0804826 seconds
Ukuran Gambar sebelum: 204014 bit
Ukuran Gambar setelah: 171786 bit
Persentase kompresi: 15.797%
Kedalaman QuadTree: 10
Jumlah Simpul: 142001

Saving GIF to: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.gif"
GIF saved successfully to: "/mnt/c/.../Tucil2_13523036_13523075/test/a/a_1_10_1.gif"
```

Program kemudian akan memproses gambar, melakukan pembagian blok berdasarkan MAD, dan menghasilkan gambar serta animasi hasil kompresi.

## Author

| Nama                 | NIM      |
| -------------------- | -------- |
| Yonatan Edward Njoto | 13523036 |
| Muhammad Dicky Isra  | 13523075 |
