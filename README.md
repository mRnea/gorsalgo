# Görsel Algoritma
Çizge ve ağaç gibi veri yapıları üzerindeki algoritmaları görselleştirme amaçlı yazılım.

## Kurulum
```bash
apt install libsdl2-dev
make
```

## Kullanım

### load

./main -load graph.txt
İçinde boyut ve komşuluk matrisi olan dosyayı aç.


### builder

./main -builder
Çizgeyi programda inşa et.
v: köşe ekle
e: kenar ekle

## Todo
+ [ ] Graph coloring algoritmasını ekle.
+ [ ] Multigraph renderla (düz çizgi yerine eğri)
