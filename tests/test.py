from pathlib import Path
import random
import subprocess
import sys

runner = Path(sys.argv[1] if len(sys.argv) > 1 else 'build/tests.exe').resolve()
count = 0

def run(task, data='', valid=True):
    global count
    result = subprocess.run([str(runner), str(task)], input=data.encode('utf-8'), capture_output=True, timeout=3)
    count += 1
    assert result.returncode == (0 if valid else 1), (task, data[:100], result.returncode, result.stderr.decode('utf-8', errors='replace'))
    return result.stdout.decode('utf-8').replace('\r\n', '\n').strip()

for x, y, expected in [(0,1,'Да'),(0,0,'На границе'),(3,0,'На границе'),(-3,0,'На границе'),(0,3,'На границе'),(4,0,'Нет'),(0,-1,'Нет'),(2,2,'Да'),(3,3,'Нет'),(1000000,1000000,'Нет')]:
    assert run(11,f'{x}\n{y}') == expected
assert run(11,'0,5\n0,5') == 'Да'
months=['Январь','Февраль','Март','Апрель','Май','Июнь','Июль','Август','Сентябрь','Октябрь','Ноябрь','Декабрь']
for n,name in enumerate(months,1): assert run(12,str(n)) == name
for row in run(21).splitlines(): assert row.split(':')[1].strip() == ' '.join(map(str,range(1,22,2)))
for data, expected in [('-1\n-1\n1',1.2),('0\n0\n1',.1),('.9\n.9\n1',1),('1\n1\n1',1/1.1**2)]:
    assert abs(float(run(22,data).splitlines()[1].split()[1])-expected)<1e-10
assert len(run(22,'0\n1\n0.3').splitlines())==5
assert len(run(22,'0\n999\n1').splitlines())==1001
assert run(31,'2\n3')=='z = 5'
a,b=-10,2
for n in range(1,25):
    if n==1: value=a
    elif n==2: value=b
    else: a,b=b,abs(a)-6*b;value=b
    assert run(32,str(n)) == f'b[{n}] = {value}'
assert run(41,'4 -7 0 12 -3 8') == '-4 -7 0 -12 -3 -8'
assert 'Среднее: 5' in run(42,'3\n1 3 5\n2 4 7\n6 8 9')
assert 'Среднее: -3' in run(42,'2\n0 -3\n5 0')
assert 'не определено' in run(42,'1\n1')
assert 'не определено' in run(42,'2\n1 2\n3 4')
for n in [1,2,12,200]:
    rows=run(51,str(n)).splitlines()
    original=list(map(int,rows[1].split()))
    ordered=list(map(int,rows[3].split()))
    assert len(original)==n and all(-99<=v<=99 for v in original) and ordered==sorted(original)
rng=random.Random(7)
for _ in range(30):
    a=[rng.randint(-100,100) for _ in range(rng.randint(1,30))]
    assert list(map(int,run(52,' '.join(map(str,a))).splitlines()[-1].split()))==sorted(a)
    assert list(map(int,run(41,' '.join(map(str,a))).split()))==[-abs(v) for v in a]
for data,expected in [('abc\nXY\n0\n0','XYabc'),('abc\nXY\n1\n0','abcXY'),('abc\nXY\n2\n1','aXYbc'),('Привет\nмир\n2\n3','Примирвет'),('\nтекст\n2\n0','текст'),('строка\n\n1\n0','строка'),('a😀b\nЖ\n2\n2','a😀Жb')]:
    assert run(61,data)==expected
assert run(61,'a'*2048+'\n'+'b'*2048+'\n1\n0')=='a'*2048+'b'*2048
filtered=run(62,'код\nЭто код\nБезПробелов\nКод\nкод\nдва слова')
assert 'Строк с подстрокой: 2' in filtered
assert filtered.endswith('Это код\nкод')
assert 'Таких строк нет.' in run(62,'x\n')
assert 'Строк с подстрокой: 0' in run(62,'x\na\tb')
assert 'a b' in run(62,'a\na b\r\nx')
for bad in ['', 'NaN', 'inf', '1e309', '2x', '1.2.3', '1,2,3', '1000001', '9'*100, '\x00']:
    run(11,bad+'\n1',False)
for bad in ['0','13','-1','1.0','1e1','+',' 1','1 ','99999999999999999999']:
    run(12,bad,False)
for bad in ['1\n0\n1','0\n1\n0','0\n1\n-1','0\n1000\n1','0\n1\n0.00000001']:
    run(22,bad,False)
for bad in ['0','25','1.5','1e2']:run(32,bad,False)
for bad in ['', '1 2 q', '1,2', '1000001', ' '.join(['1']*201)]:run(41,bad,False)
for bad in ['2\n1 2\n3','2\n1 2 3\n4 5','1\n1\n2','0\n','21\n','2\n1 2']:
    run(42,bad,False)
for bad in ['0','201','-1','q']:run(51,bad,False)
run(52,' '.join(['1']*31),False)
for bad in ['abc\nx\n2\n4','abc\nx\n3\n0','a'*2049+'\nx\n1\n0','abc\nx\n2\n-1']:
    run(61,bad,False)
for bad in ['\ntext','x\na\x00b','x\n'+'a'*262145]:run(62,bad,False)
print(f'PASS: {count} algorithm, boundary, validation and randomized checks')
