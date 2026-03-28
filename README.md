# Voxelisasi Mesh 3D Berbasis Octree

## Penjelasan Program

Program ini melakukan **voxelisasi objek 3D** dengan membaca file mesh (format OBJ) dan mengkonversinya menjadi representasi voxel menggunakan struktur data **Octree**. 

Algoritma yang digunakan adalah **Divide and Conquer**, di mana ruang 3D dibagi secara rekursif menjadi 8 octant, lalu dilakukan pengecekan irisan antara segitiga mesh dengan setiap cell untuk menentukan voxel mana yang terisi. Hasil akhir disimpan dalam format OBJ.

## Requirement dan Instalasi

### Requirement Sistem
- **Compiler**: GCC atau MSVC dengan dukungan C++17
- **Library**: Standard C++ (iostream, vector, string, filesystem, fstream, cmath, algorithm, map, sstream, array, tuple)
- **OS**: Windows atau Linux

### Instalasi
Tidak memerlukan instalasi library eksternal. Semua dependency adalah bagian dari standard C++.

## Cara Mengkompilasi

### Menggunakan g++
```bash
cd path/ke/tucil2
g++ -std=c++17 -o bin/main src/main/main.cpp src/models/Base.cpp src/models/Octree.cpp src/models/Parser.cpp -I src/
```

### Menggunakan MSVC (Visual Studio)
Gunakan IDE atau compile dengan:
```bash
cl /std:c++latest /Fo bin/ /Fe bin/main.exe src/main/main.cpp src/models/Base.cpp src/models/Octree.cpp src/models/Parser.cpp /I src/
```

## Cara Menjalankan dan Menggunakan Program

### Langkah Menjalankan

1. Pastikan file testcase ada di `src/test/testcase/`
2. Jalankan program:
   ```bash
   ./bin/main
   ```

3. Program akan meminta input:
   - **Input file name**: Masukkan nama file OBJ (misalnya: `pumpkin.obj`)
   - **Input octree depth**: Masukkan kedalaman octree (bilangan >= 0, misalnya: 5)

### Contoh Eksekusi
```
Input file name from src/test/testcase: pumpkin.obj
Input octree depth (>= 0): 5
Voxelization complete.
Voxels: 1234
Vertices: 5678
Faces: 9012
Output: src/test/result/pumpkin_voxel_d5.obj
```

### Output
- **File OBJ**: Disimpan di `src/test/result/` dengan nama format `[filename]_voxel_d[depth].obj`
- **Statistik**: Program menampilkan jumlah voxel, vertex, dan faces hasil voxelisasi

## Author / Identitas Pembuat

**Nama**: Mahmudia Kimdaro Amin 
**NIM**: 13524083    

