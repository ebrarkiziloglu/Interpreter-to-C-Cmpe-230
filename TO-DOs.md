
//// TO-DOs:

- IDs list hes compilationda sıfırlıyor muyuz? Sıfırlayabiliriz, memset vs
- "Should we detect and print an error message when a scalar variable is followed by square brackets? (Or when a vector is followed by a matrix like indexing)" YES dedi Hoca.
- documentation + comment
- Hocadan caldığımız kodu bir tık özelleştirebiliriz
- ERROR lineımızda syntax farklılığı var - piazza son soruya verilen cevap!!
- print, printsep, matrix etc are not valid id names
- isID fonksiyonunun içindeki for u MAXID ye kadar götürmek yerine currentidye kadar götürmek
- valgrind
- Just use the floor function  to convert float to int
- printsep length -- take it as 12
- https://piazza.com/class/kzpxjob9qs01nt?cid=59



FOR TEST CASES: 

```
//valid
vector y[2]
y = {1 2}
y[1] = 3
```

```
Scalar[id]
```

```
Matrix [id]
```

```
A[sqrt(4)], A[i+5-2+j], or A[choose(i,1,2,3)]
```

```
scalar  x
vector x[2]
// error
```

```
scalar  x
scalar  x
// error
```

```
//valid
vector v[5]
matrix m[4,5]
v[1] = 5
m[1,2] = 3
```

```
//invalid
vector v[5]
matrix m[4,5]
m[2] = v
m[3] = tr(v)
```

```
//all pf them are invalid
scalar s
vector v
print(s[1])
print(s[2])
print(v[1,1])
```

```
//INVALID
matrix A[3,2]
matrix row[1,2]
row = A[1]
```

```
//INVALID
scalar A
matrix B[1,2]
B = { 1 1 }
A = (B*tr(B))[1,1]
```

```
matrix A[1,2]
matrix B[1,2]
A = { 1 1 }
B = A
A[1,1] = 2
print(B[1,1])
//this should print 1
```
NOTES:
- Can you provide us with a hard upper limit for the number of characters in a line and the number of lines in the .mat file? 256
- 




- ~~Matlangde index 1 dediği aslında 0.~~
- ~~Ve for döngüsünde upper bound dediği expr2 dahil, o yüzden küçük eşit dememiz lazım~~
- ~~Bir de for un içinde i yi float olarak tanımlamışız onu da int yapalım~~
- ~~Otomatik printf kullanirken intleri de float olarak yazdırıyoruz. Bunun kontrolü gerekiyor.~~
- ~~factor fonksiyonunda id lerden sonra, 'str' ye type ekleyecegiz.~~
~~2. During definitions, create struct and add id names into 'IDs'.~~
~~3. Read assingments, first token: id, second token: =, rest is: <expr>~~
4. Functions in the file.c:\
~~&nbsp;&nbsp;&nbsp;** Scalar addition\
&nbsp;&nbsp;&nbsp;** Scalar multiplication\
&nbsp;&nbsp;&nbsp;** Scalar substraction\
&nbsp;&nbsp;&nbsp;** Vector addition\
&nbsp;&nbsp;&nbsp;** Vector multiplication\
&nbsp;&nbsp;&nbsp;** Vector substraction\
&nbsp;&nbsp;&nbsp;** Scalar * Vector\
&nbsp;&nbsp;&nbsp;** Scalar * Matrix\
&nbsp;&nbsp;&nbsp;** Matrix addition\
&nbsp;&nbsp;&nbsp;** Matrix multiplication\
&nbsp;&nbsp;&nbsp;** Matrix substraction\
&nbsp;&nbsp;&nbsp;** transpose of Matrix\
&nbsp;&nbsp;&nbsp;** transpose of Vector\
&nbsp;&nbsp;&nbsp;** transpose of Scalar\
&nbsp;&nbsp;&nbsp;** sqrt of scalar\
&nbsp;&nbsp;&nbsp;** choose (expr, expr, expr, expr)\
&nbsp;&nbsp;&nbsp;** Print matrix\
&nbsp;&nbsp;&nbsp;** Print vector\
&nbsp;&nbsp;&nbsp;** Print scalar\
&nbsp;&nbsp;&nbsp;** Printsep\
&nbsp;&nbsp;&nbsp;** CopyMatrix~~\
  
~~6. for loop~~
  
~~7. matrix dimension check with ID type~~
  
~~8. id name check (_alphanumeric etc.)~~
  
~~9. assign statement, factor before = --> A[2,2] A[2] A --> function like process~~
  
~~12. read line into process~~
  
~~13. print(Error Line) line number~~
 
