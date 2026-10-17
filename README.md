# tugas-modul-2
Program ini menggunakan ROS 2 dengan node berbasis bahasa C++ untuk menggerakkan kura-kura (turtlesim) agar membentuk gambar outline gelas kopi. 
Node ini mengombinasikan tiga elemen komunikasi ROS 2 sekaligus:   

Subscriber: Membaca posisi kura-kura secara real-time dari topik /turtle1/pose.

Publisher: Mengirim kecepatan gerak (/turtle1/cmd_vel) dengan kontrol navigasi proporsional agar kura-kura tepat menuju titik-titik koordinat target.

Service Client: Memanggil layanan /turtle1/set_pen untuk mengatur warna putih, ketebalan, serta mengangkat/menurunkan pena (off = 1 atau off = 0) agar 
garis antarbagian tidak "bocor" atau bercoretan saat kura-kura berpindah posisi.

video penjelasan : https://drive.google.com/file/d/1iSkpOFI40OElwshJHSUVKj60o5gPp9Ma/view?usp=sharing
hasil gambar : https://drive.google.com/file/d/1lW8nIkcjyVDr_-SQ6Q2lyjgXpgkwcCuu/view?usp=sharing
