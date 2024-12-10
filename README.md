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
Çizgeyi programda inşa et.

+ v: köşe ekle
+ e: kenar ekle
+ v: köşe ekle.
+ e: kenar ekle.
+ g: pgraph -> graph.
+ k: koordinatları kaydet.
+ s: backtracking solve.
+ x: greedy solve.
+ r: köşe yarıçapını arttır.
+ R: köşe yarıçapını azalt.
+ a: history geri.
+ d: history ileri.
+ A: history önceki çözüm.
+ D: history sonraki çözüm.

## Todo
+ [x] Graph coloring algoritmasını ekle.
+ [ ] Multigraph renderla (düz çizgi yerine eğri)
+ [ ] Ağaç ekle
