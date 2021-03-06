/* vim:set ts=2 sw=2 sts=2 et: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Tests that exceptions thrown by content don't show up twice in the Web
// Console.

const TEST_DUPLICATE_ERROR_URI = "http://example.com/browser/browser/devtools/webconsole/test/test-duplicate-error.html";

function test() {
  addTab("data:text/html;charset=utf8,hello world");
  browser.addEventListener("load", function onLoad() {
    browser.removeEventListener("load", onLoad, true);
    openConsole(null, consoleOpened);
  }, true);

  function consoleOpened(hud)
  {
    expectUncaughtException();
    content.location = TEST_DUPLICATE_ERROR_URI;

    waitForMessages({
      webconsole: hud,
      messages: [{
        text: "fooDuplicateError1",
        category: CATEGORY_JS,
        severity: SEVERITY_ERROR,
      },
      {
        text: "test-duplicate-error.html",
        category: CATEGORY_NETWORK,
        severity: SEVERITY_LOG,
      }],
    }).then(() => {
      let text = hud.outputNode.textContent;
      let error1pos = text.indexOf("fooDuplicateError1");
      ok(error1pos > -1, "found fooDuplicateError1");
      if (error1pos > -1) {
        ok(text.indexOf("fooDuplicateError1", error1pos + 1) == -1,
          "no duplicate for fooDuplicateError1");
      }

      finishTest();
    });
  }
}
