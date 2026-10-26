const $ = (id) => document.getElementById(id);
const icons = ["⟐", "↻", "ƒ", "▦", "⇅", "{ }"];
const descriptions = [
  "Области на плоскости и выбор месяца",
  "Три вида циклов и таблица функции",
  "Минимум и рекурсивная последовательность",
  "Преобразование массива и матрицы",
  "Указатели и сортировка вставками",
  "Char-строки и обработка текстового файла",
];
let current = null,
  selectedLab = 0,
  started = 0,
  pending = false,
  animation = true;
const drafts = new Map();
const escapeHtml = (s) =>
  String(s).replace(
    /[&<>"']/g,
    (c) =>
      ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" })[
        c
      ],
  );
function highlight(source) {
  return source
    .split("\n")
    .map(
      (line) =>
        '<span class="code-line">' +
        line
          .split(
            /("(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|\b(?:void|int|double|long|char|const|return|if|else|while|do|for|auto|size_t|inline|try|catch|throw|bool|static_cast|sizeof|true|false)\b|\b\d+(?:\.\d+)?\b)/g,
          )
          .map((token) => {
            const value = escapeHtml(token);
            if (/^["']/.test(token))
              return '<span class="tok-string">' + value + "</span>";
            if (/^\d/.test(token))
              return '<span class="tok-num">' + value + "</span>";
            if (
              /^(void|int|double|long|char|const|return|if|else|while|do|for|auto|size_t|inline|try|catch|throw|bool|static_cast|sizeof|true|false)$/.test(
                token,
              )
            )
              return '<span class="tok-key">' + value + "</span>";
            return value;
          })
          .join("") +
        "</span>",
    )
    .join("");
}
function notify(text) {
  $("toast").textContent = text;
  $("toast").classList.add("show");
  clearTimeout(notify.timer);
  notify.timer = setTimeout(() => $("toast").classList.remove("show"), 2600);
}
function send(message) {
  if (window.chrome?.webview) window.chrome.webview.postMessage(message);
  else {
    pending = false;
    if ($("run")) $("run").disabled = false;
    notify("Вычисления доступны в CyberLab.exe.");
  }
}
function remember() {
  if (current)
    drafts.set(
      current.id,
      current.fields.map((f) => $("f-" + f[0])?.value ?? f[2]),
    );
}
function selectNav(n) {
  selectedLab = n;
  document
    .querySelectorAll(".nav-item")
    .forEach((b, i) => b.classList.toggle("active", i + 1 === n));
  $("overview").classList.toggle("active", n === 0);
}
function home() {
  remember();
  current = null;
  selectNav(0);
  $("breadcrumb").innerHTML = "Рабочее пространство <span>/</span> Обзор";
  $("content").innerHTML =
    `<section class="hero enter"><div class="eyebrow">ЛИЧНАЯ ЛАБОРАТОРИЯ / ВАРИАНТ 01</div><h1>Код. Практика.<br><em>Результат.</em></h1><p>Шесть лабораторных работ по C++ в одном пространстве.<br>Запускай решения, изучай код и разбирай алгоритмы.</p><div class="hero-tags"><span>Сагадатов Динияр</span><span>ИФБС-11</span><span>Зачётка №1</span></div><div class="orb"><svg viewBox="0 0 80 90" fill="none" aria-hidden="true"><path d="M40 4L72 16V43C72 62 57 76 40 86C23 76 8 62 8 43V16L40 4Z" stroke="#67e6c9" stroke-width="1.5" fill="#3fc9ab0b"/><path d="M29 35L19 45L29 55M51 35L61 45L51 55M44 30L36 60" stroke="#7be5cf" stroke-width="2"/></svg><div class="orb-label">LEARN · RUN · UNDERSTAND</div></div></section><div class="stats enter"><div class="stat"><strong>06</strong><small>лабораторных<br>работ</small></div><div class="stat"><strong>12</strong><small>учебных модулей<br>11 заданий + практика</small></div><div class="stat"><strong>01</strong><small>вариант<br>во всех работах</small></div></div><div class="section-title"><h2>Выберите лабораторную</h2><small>Теория через практику</small></div><div class="grid">${Array.from({ length: 6 }, (_, i) => `<button class="lab-card" data-lab="${i + 1}"><div class="card-top"><span class="card-icon">${icons[i]}</span><span class="card-number">LAB / 0${i + 1}</span></div><h3>${TASKS[i * 2].lab}</h3><p>${descriptions[i]}</p><div class="card-bottom"><span>${i === 4 ? "1 задание + практика" : "2 задания"} · Вариант 1</span><b>↗</b></div></button>`).join("")}</div><div class="notice">В лабораторной №5 исходный PDF содержит одно задание; второй модуль — дополнительная практика. В №6 выбран первый из двух пунктов с номером «1».</div>`;
  document
    .querySelectorAll("[data-lab]")
    .forEach((b) => (b.onclick = () => showTask(b.dataset.lab + "1")));
  window.scrollTo(0, 0);
}
function fieldHtml(f, index) {
  const value = drafts.get(current.id)?.[index] ?? f[2];
  const id = "f-" + f[0];
  const wide = f[3] === "area" || ["first", "second", "search"].includes(f[0]);
  let control;
  if (f[3] === "area")
    control = `<textarea id="${id}" maxlength="${current.id === "62" ? 262144 : 16384}" spellcheck="false">${escapeHtml(value)}</textarea>`;
  else if (f[3] === "select")
    control = `<select id="${id}">${["В начало", "В конец", "В заданную позицию"].map((s, i) => `<option value="${i}" ${String(value) === String(i) ? "selected" : ""}>${s}</option>`).join("")}</select>`;
  else
    control = `<input id="${id}" type="text" value="${escapeHtml(value)}" maxlength="${["first", "second", "search"].includes(f[0]) ? 2048 : 64}" autocomplete="off" spellcheck="false">`;
  return `<div class="field ${wide ? "wide" : ""}"><label for="${id}">${escapeHtml(f[1])}</label>${control}</div>`;
}
function showTask(id) {
  if (pending) return;
  remember();
  current = TASKS.find((t) => t.id === id);
  selectNav(Number(id[0]));
  $("breadcrumb").innerHTML =
    `Лабораторные работы <span>/</span> 0${id[0]} <span>/</span> ${id === "52" ? "Практика" : "Задание " + id[1]}`;
  $("content").innerHTML =
    `<div class="enter"><div class="page-title"><div><div class="eyebrow">ЛАБОРАТОРНАЯ / 0${id[0]}</div><h1>${current.lab}</h1><p>Вариант 1 · ${current.subtitle}</p></div><div class="lab-badge">0${id[0]}</div></div><div class="tabs"><button class="tab ${id[1] === "1" ? "active" : ""}" data-task="${id[0]}1">Задание 01</button><button class="tab ${id[1] === "2" ? "active" : ""}" data-task="${id[0]}2">${id[0] === "5" ? "Практика 02 <small>ДОПОЛНИТЕЛЬНО</small>" : "Задание 02"}</button></div><section class="task-intro"><h2>${current.title}</h2><p>${current.description}</p><div class="formula">${current.formula}</div>${current.note ? '<p class="lab-note">' + current.note + "</p>" : ""}</section><div class="work-grid"><section class="panel"><div class="panel-heading"><h3>Входные данные</h3><small>INPUT / ${id}</small></div><form class="panel-body" id="task-form"><div class="fields">${current.fields.map(fieldHtml).join("")}</div>${!current.fields.length ? '<p class="limits">Три способа перебора одной последовательности. Нажмите «Выполнить», чтобы сравнить результаты.</p>' : ""}<p class="limits">${current.limits}</p><div class="actions"><button class="primary" id="run" type="submit">▷ &nbsp; Выполнить</button><button class="secondary" id="reset" type="button">Пример</button>${id === "62" ? '<button class="secondary" id="open-file" type="button">Открыть .txt</button>' : ""}</div></form></section><section class="panel"><div class="panel-heading"><h3>Результат выполнения</h3><span class="result-status" id="result-status">ОЖИДАНИЕ</span></div><div class="panel-body">${id === "11" ? '<div id="plot"></div>' : ""}<pre id="result" class="result placeholder" role="status" aria-live="polite">Здесь появится результат.\nПроверьте входные данные и запустите решение.</pre><div class="result-time" id="result-time">C++17 · Вычисления на вашем компьютере</div>${id === "62" ? '<button class="secondary" id="save-file" disabled style="margin-top:14px">Сохранить отобранные строки</button>' : ""}</div></section></div><div class="jump-row"><a class="jump" href="#full-code">↓ Полный код</a><a class="jump" href="#explanations">↓ Разбор по блокам</a></div><section class="panel code-panel" id="full-code"><div class="panel-heading"><h3>Полный код решения</h3><div class="code-toolbar"><small>lab${id}.cpp · БЕЗ КОММЕНТАРИЕВ</small><button id="copy-code">Копировать</button></div></div><pre class="code-view"><code>${highlight(current.code)}</code></pre></section><section class="explanations" id="explanations"><div class="section-title"><h2>Как это работает</h2><small>Три смысловых блока</small></div>${current.blocks.map((block, i) => `<article class="explain-card"><span class="step-no">0${i + 1}</span><div><h3>${["Подготовка и проверка данных", "Алгоритм решения", "Запуск и обработка ошибок"][i]}</h3><p>${current.explain[i]}</p><pre class="snippet">${escapeHtml(block)}</pre></div></article>`).join("")}</section></div>`;
  document
    .querySelectorAll("[data-task]")
    .forEach((b) => (b.onclick = () => showTask(b.dataset.task)));
  $("task-form").onsubmit = (e) => {
    e.preventDefault();
    run();
  };
  $("reset").onclick = () => {
    current.fields.forEach((f) => ($("f-" + f[0]).value = f[2]));
    invalidate();
    modeState();
  };
  $("copy-code").onclick = () => send("copy\n" + current.code);
  if (id === "62") {
    $("open-file").onclick = () => send("open\n");
    $("save-file").onclick = () => send("save\n");
  }
  $("task-form").addEventListener("input", invalidate);
  if (id === "61")
    $("f-mode").onchange = () => {
      modeState();
      invalidate();
    };
  modeState();
  if (id === "11") drawPoint();
  window.scrollTo(0, 0);
}
function modeState() {
  if (current?.id === "61")
    $("f-position").disabled = $("f-mode").value !== "2";
}
function invalidate() {
  if ($("save-file")) $("save-file").disabled = true;
  if ($("result-status")?.classList.contains("ok")) {
    $("result-status").textContent = "ДАННЫЕ ИЗМЕНЕНЫ";
    $("result-status").className = "result-status";
  }
}
function drawPoint() {
  if (!$("plot")) return;
  const x = Number($("f-x").value.replace(",", ".")),
    y = Number($("f-y").value.replace(",", "."));
  const visible =
    Number.isFinite(x) &&
    Number.isFinite(y) &&
    Math.abs(x) <= 4 &&
    Math.abs(y) <= 4;
  $("plot").innerHTML =
    `<svg class="result-chart" viewBox="0 0 280 140" aria-label="Верхний полукруг радиуса три"><defs><pattern id="grid" width="28" height="28" patternUnits="userSpaceOnUse"><path d="M28 0H0V28" fill="none" stroke="#22394b" stroke-width=".5"/></pattern></defs><rect width="280" height="140" fill="url(#grid)"/><path d="M56 105A84 84 0 0 1 224 105Z" fill="#54e2b522" stroke="#4da991"/><path d="M15 105H265M140 8V137" stroke="#59738e"/><text x="49" y="122" fill="#8aa2b9" font-size="10">−3</text><text x="222" y="122" fill="#8aa2b9" font-size="10">3</text><text x="145" y="22" fill="#8aa2b9" font-size="10">3</text><text x="255" y="99" fill="#8aa2b9" font-size="10">x</text><text x="147" y="10" fill="#8aa2b9" font-size="10">y</text>${visible ? `<circle cx="${140 + x * 28}" cy="${105 - y * 28}" r="4" fill="#67f6cd" stroke="#fff" stroke-width="1"/>` : ""}</svg>`;
}
function run() {
  if (pending) return;
  const values = current.fields.map((f) => $("f-" + f[0]).value);
  if (current.id === "61" && values[2] !== "2") values[3] = "0";
  const payload = values.join("\n");
  if (new TextEncoder().encode(payload).length > 270000) {
    showResult(false, "Ввод превышает допустимый размер.");
    return;
  }
  pending = true;
  started = performance.now();
  $("run").disabled = true;
  $("result-status").textContent = "ВЫЧИСЛЕНИЕ";
  if ($("save-file")) $("save-file").disabled = true;
  send("run" + current.id + "\n" + payload);
}
function showResult(ok, text) {
  pending = false;
  if (!$("result")) return;
  $("run").disabled = false;
  $("result").className = "result" + (ok ? "" : " error");
  $("result").textContent = text;
  $("result-status").className = "result-status " + (ok ? "ok" : "error");
  $("result-status").textContent = ok ? "ГОТОВО" : "ПРОВЕРЬТЕ ВВОД";
  $("result-time").textContent = ok
    ? "Выполнено · " +
      Math.max(1, Math.round(performance.now() - started)) +
      " мс"
    : "Исправьте данные и попробуйте снова";
  if ($("save-file")) $("save-file").disabled = !ok;
  if (ok && current.id === "11") drawPoint();
}
if (window.chrome?.webview)
  window.chrome.webview.addEventListener("message", (event) => {
    const message = event.data,
      split = message.indexOf("\n"),
      type = message.slice(0, split),
      body = message.slice(split + 1);
    if (type === "ok" || type === "error") showResult(type === "ok", body);
    else if (type === "file" && current?.id === "62") {
      $("f-text").value = body;
      invalidate();
      notify("Файл UTF-8 загружен");
    } else if (type === "notice") notify(body);
  });
$("nav").innerHTML = Array.from(
  { length: 6 },
  (_, i) =>
    `<button class="nav-item" data-n="${i + 1}"><span class="num">0${i + 1}</span><span>${TASKS[i * 2].lab}<small>${descriptions[i].split(" и ")[0]}</small></span></button>`,
).join("");
document.querySelectorAll("[data-n]").forEach(
  (b) =>
    (b.onclick = () => {
      if (!pending) showTask(b.dataset.n + "1");
    }),
);
$("home").onclick = $("overview").onclick = () => {
  if (!pending) home();
};
$("motion").onclick = () => {
  animation = !animation;
  document.body.classList.toggle("no-motion", !animation);
  $("motion").setAttribute(
    "aria-label",
    animation ? "Выключить анимации" : "Включить анимации",
  );
  notify(animation ? "Анимации включены" : "Анимации выключены");
};
home();
