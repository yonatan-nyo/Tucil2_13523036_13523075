# QuadTree Image Error Calculation

## Deskripsi Singkat

Program ini mengimplementasikan algoritma QuadTree untuk membagi gambar dan menghitung error berdasarkan beberapa metode statistik seperti Variance, Mean Absolute Deviation, Max Pixel Difference, dan Entropy. Program ini dikembangkan menggunakan C++.

## Requirement & Instalasi

- Compiler C++ (g++ dengan dukungan C++17)
- Valgrind (opsional, untuk debugging memori)
- Make
- WSL (jika menggunakan windows, supaya bisa memanfaatkan Makefile)

Jika menggunakan sistem berbasis Unix seperti ubuntu, install g++ dan Valgrind (optional untuk debug) dengan:

update apt-get dahulu

```sh
sudo apt-get update
```

install g++

```sh
sudo apt-get install build-essential gdb
```

install valgrind

```sh
sudo apt install valgrind
```

## Cara Menjalankan

Gunakan Makefile yang telah disediakan untuk mengkompilasi dan menjalankan program dengan:

```sh
make all
```

Untuk menjalankan program dengan Valgrind guna mendeteksi kebocoran memori:

```sh
make debug
```

## Author

- 13523036 - Yonatan Edward Njoto
- 13523075 - Muhammad Dicky Isra
