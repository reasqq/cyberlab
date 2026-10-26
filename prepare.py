from pathlib import Path
import json
import re

root = Path(__file__).resolve().parent
common = (root / 'labs/common.hpp').read_text(encoding='utf-8')
headers = '\n'.join(s for s in common.splitlines() if s.startswith('#include'))
helpers = {}
for match in re.finditer(r'inline [^\n]+? (\w+)\([^\n]*\) \{\n.*?\n\}', common, re.S):
    helpers[match.group(1)] = match.group(0)
tasks = json.loads((root / 'ui/tasks.json').read_text(encoding='utf-8'))
(root / 'standalone').mkdir(exist_ok=True)
for task in tasks:
    task_id = task['id']
    body = (root / f'labs/lab{task_id}.cpp').read_text(encoding='utf-8').replace('#include "common.hpp"\n', '').strip()
    needed = set()
    previous = None
    combined = body
    while previous != needed:
        previous = needed.copy()
        needed |= {name for name in helpers if re.search(r'\b' + name + r'\(', combined)}
        combined = body + '\n'.join(helpers[name] for name in needed)
    validation = '\n\n'.join(code for name, code in helpers.items() if name in needed)
    main = f'''int main() {{
    try {{
        lab{task_id}(std::cin, std::cout);
        return 0;
    }} catch (const std::exception& error) {{
        std::cerr << "Ошибка: " << error.what() << '\\n';
        return 1;
    }}
}}'''
    if task_id == '62':
        main = '''int main() {
    try {
        std::string sourcePath = line(std::cin);
        std::string targetPath = line(std::cin);
        std::string search = line(std::cin, 2048);
        std::ifstream source(sourcePath, std::ios::binary);
        require(bool(source), "Не удалось открыть исходный файл.");
        std::ifstream existing(targetPath, std::ios::binary);
        require(!existing, "Выходной файл уже существует. Выберите новое имя.");
        std::string selected = filterText(source, std::cout, search);
        std::ofstream target(targetPath, std::ios::binary);
        require(bool(target), "Не удалось создать выходной файл.");
        target << selected;
        target.close();
        require(bool(target), "Ошибка записи файла.");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\\n';
        return 1;
    }
}'''
    source = '\n\n'.join(part for part in [headers, validation, body, main] if part) + '\n'
    task['code'] = source
    task['blocks'] = [validation or headers, body, main]
    task['explain'] = [
        ('Общие функции читают строки с ограничением длины, проверяют тип и диапазон числа и сообщают об ошибке через require. Подключены только заголовки стандартной библиотеки C++. ' + task['limits']) if validation else 'Подключены заголовки стандартной библиотеки C++. В этой задаче пользовательский ввод отсутствует, поэтому функции проверки не нужны.',
        ' '.join(task['explain']),
        ('main читает пути исходного и нового файла, а затем искомую подстроку. Существующий выходной файл не перезаписывается. Ошибки открытия, чтения и записи превращаются в сообщения.') if task_id == '62' else 'main передаёт стандартные потоки ввода и вывода функции решения. При успехе возвращает 0; при ошибке показывает её причину и возвращает 1. В графическом приложении та же функция получает данные из полей формы.'
    ]
    (root / f'standalone/lab{task_id}.cpp').write_text(source, encoding='utf-8')
html = (root / 'ui/index.html').read_text(encoding='utf-8')
html = html.replace('/*__DATA__*/', 'const TASKS = ' + json.dumps(tasks, ensure_ascii=False).replace('</', '<\\/') + ';')
html = html.replace('/*__CSS__*/', (root / 'ui/style.css').read_text(encoding='utf-8'))
html = html.replace('/*__JS__*/', (root / 'ui/app.js').read_text(encoding='utf-8'))
(root / 'ui/preview.html').write_text(html, encoding='utf-8')
(root / 'src/ui.hpp').write_text('const char* HTML = R"CYBERLAB(' + html + ')CYBERLAB";\n', encoding='utf-8')
print('Prepared 12 standalone C++ sources and embedded interface.')
