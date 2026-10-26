const { chromium } = require("playwright");
const assert = require("node:assert/strict");
const path = require("node:path");

(async () => {
  const browser = await chromium.connectOverCDP(process.env.CYBERLAB_CDP || "http://127.0.0.1:9333");
  const page = browser.contexts().flatMap((c) => c.pages())[0];
  const errors = [];
  page.on("pageerror", (error) => errors.push(error.message));
  await page.locator('#home').click();
  await page.waitForSelector('[data-lab="1"]');
  assert.equal(await page.locator("[data-lab]").count(), 6);
  await page.screenshot({
    path: path.resolve("build/preview-home.png"),
    fullPage: true,
  });
  const expected = {
    11: "Да",
    12: "Октябрь",
    21: "21",
    22: "x",
    31: "z = 5",
    32: "506",
    41: "-4 -7 0 -12 -3 -8",
    42: "Среднее: 5",
    51: "После сортировки",
    52: "1 3 4 7 9",
    61: "безопасная Киберлаборатория",
    62: "Строк с подстрокой: 2",
  };
  for (const [id, text] of Object.entries(expected)) {
    await page.locator(`[data-n="${id[0]}"]`).click();
    if (id[1] === "2") await page.locator(`[data-task="${id}"]`).click();
    await page.locator("#run").click();
    await page.waitForFunction(
      () => document.getElementById("result-status").textContent === "ГОТОВО",
    );
    assert.ok((await page.locator("#result").innerText()).includes(text), id);
    assert.ok(
      (await page.locator("#full-code").innerText()).includes("int main()"),
      id,
    );
    assert.equal(await page.locator(".explain-card").count(), 3);
    assert.equal(
      await page.evaluate(
        () => document.documentElement.scrollWidth > window.innerWidth,
      ),
      false,
      id,
    );
    if (id === "11")
      await page.screenshot({ path: path.resolve("build/preview-task.png") });
    console.log(`PASS GUI ${id}`);
  }
  await page.locator('[data-n="1"]').click();
  await page.locator("#f-x").fill("NaN");
  await page.locator("#run").click();
  await page.waitForFunction(
    () =>
      document.getElementById("result-status").textContent === "ПРОВЕРЬТЕ ВВОД",
  );
  await page.locator("#reset").click();
  await page.locator("#run").click();
  await page.waitForFunction(
    () => document.getElementById("result-status").textContent === "ГОТОВО",
  );
  await page.locator("#copy-code").click();
  await page.waitForFunction(() =>
    document.getElementById("toast").textContent.includes("скопирован"),
  );
  await page.locator('a[href="#explanations"]').click();
  await page.waitForFunction(() => window.scrollY > 1000);
  assert.equal(await page.title(), "CyberLab — Сагадатов Динияр");
  await page.locator('[data-n="6"]').click();
  await page.locator("#f-first").fill("<script>alert(1)</script>");
  await page.locator("#f-second").fill("текст");
  await page.locator("#run").click();
  await page.waitForFunction(
    () => document.getElementById("result-status").textContent === "ГОТОВО",
  );
  assert.ok(
    (await page.locator("#result").innerText()).includes(
      "<script>alert(1)</script>",
    ),
  );
  assert.equal(await page.locator("#result script").count(), 0);
  await page.locator('[data-task="62"]').click();
  await page.locator("#run").click();
  await page.waitForFunction(
    () => document.getElementById("result-status").textContent === "ГОТОВО",
  );
  assert.equal(await page.locator("#save-file").isDisabled(), false);
  await page.locator("#f-search").fill("новый");
  assert.equal(await page.locator("#save-file").isDisabled(), true);
  await page.locator("#home").click();
  await page.locator("#motion").click();
  assert.equal(await page.locator("body.no-motion").count(), 1);
  assert.deepEqual(errors, []);
  await browser.close();
  console.log(
    "PASS: 12 native UI runs; recovery, copying, code navigation, escaped output, file state, animation toggle.",
  );
})().catch((error) => {
  console.error(error);
  process.exit(1);
});
