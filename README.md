# Görsel Algoritma
Çizge ve ağaç gibi veri yapıları üzerindeki algoritmaları görselleştirme amaçlı yazılım.

## Kurulum

### Linux

```bash
apt install libsdl2-dev libsdl2-ttf-dev
make
```

### Windows

+ C derleyicisi icin MinGW indirin.
+ SDL headerlarini indirip build.bat dosyasina uygun yere koyun

## Kullanım

### load

```bash
./main -load graph.txt
```
İçinde boyut ve komşuluk matrisi olan dosyayı aç.


### builder

```bash
./main -builder
```
Çizgeyi programda inşa et.\
v: köşe ekle\
e: kenar ekle

## Todo
+ [ ] Graph coloring algoritmasını ekle.
+ [ ] Multigraph renderla (düz çizgi yerine eğri)
