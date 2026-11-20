import fs from 'fs';
import path from 'path';
import puppeteer from 'puppeteer';
import { PDFDocument, StandardFonts, rgb } from 'pdf-lib';

const BASE_URL = 'http://localhost:3000/iotgateway-docs/';
const OUTPUT_DIR = path.resolve('build/pdf');
const OUTPUT_PDF = path.join(OUTPUT_DIR, 'MGW_User_Manual.pdf');

function readImageAsBase64(relPath) {
  const fullPath = path.resolve(relPath);
  if (!fs.existsSync(fullPath)) return null;
  const ext = path.extname(fullPath).slice(1).toLowerCase();
  const mime = ext === 'svg' ? 'image/svg+xml' : `image/${ext === 'jpg' ? 'jpeg' : ext}`;
  return `data:${mime};base64,${fs.readFileSync(fullPath).toString('base64')}`;
}

async function waitForServer(url, maxAttempts = 30) {
  for (let i = 0; i < maxAttempts; i++) {
    try {
      const res = await fetch(url);
      if (res.ok) return true;
    } catch {
      // Server still starting
    }
    await new Promise((r) => setTimeout(r, 1000));
  }
  throw new Error(`Timed out waiting for Docusaurus server at ${url}`);
}

async function generateCoverPdf(browser) {
  const logoSrc = readImageAsBase64('static/img/logo.png');
  const moduleSrc = readImageAsBase64('static/img/mgw_module.png');
  const dateStr = new Date().toLocaleDateString('en-US', {
    year: 'numeric',
    month: 'long',
    day: 'numeric',
  });

  const coverHtml = `
    <!DOCTYPE html>
    <html>
      <head>
        <style>
          body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            margin: 0;
            padding: 60px 50px;
            color: #0f172a;
            display: flex;
            flex-direction: column;
            justify-content: space-between;
            height: 92vh;
            box-sizing: border-box;
          }
          .top-bar {
            border-bottom: 4px solid #0284c7;
            padding-bottom: 24px;
            display: flex;
            justify-content: space-between;
            align-items: center;
          }
          .logo { height: 42px; }
          .doc-type { font-size: 13px; font-weight: 700; color: #64748b; letter-spacing: 1.5px; text-transform: uppercase; }
          .hero { margin-top: 50px; }
          h1 { font-size: 38px; line-height: 1.15; margin: 0 0 16px; color: #0f172a; }
          .subtitle { font-size: 18px; color: #475569; margin: 0 0 40px; line-height: 1.5; }
          .module-box { text-align: center; margin: 30px 0; }
          .module-img { max-width: 260px; border-radius: 12px; box-shadow: 0 12px 30px rgba(0,0,0,0.12); }
          .footer {
            border-top: 1px solid #cbd5e1;
            padding-top: 20px;
            font-size: 13px;
            color: #475569;
            line-height: 1.6;
          }
        </style>
      </head>
      <body>
        <div>
          <div class="top-bar">
            ${logoSrc ? `<img class="logo" src="${logoSrc}" alt="CONSTH" />` : '<strong>CONSTH Vietnam</strong>'}
            <span class="doc-type">Technical User Manual</span>
          </div>
          <div class="hero">
            <h1>Mini IoT Gateway<br/>(MGW-IoT-001 / MGW-IoT-002)</h1>
            <p class="subtitle">
              Official Hardware Interface, Communication Protocols, Host MCU C SDK Integration &amp; Command Reference Manual
            </p>
          </div>
          ${moduleSrc ? `<div class="module-box"><img class="module-img" src="${moduleSrc}" /></div>` : ''}
        </div>
        <div class="footer">
          <strong>Manufactured &amp; Distributed By:</strong> Công ty Cổ phần Thương mại CONSTH (CONSTH Vietnam)<br/>
          <strong>Corporate Portal:</strong> https://consth.vn/<br/>
          <strong>Online Documentation:</strong> https://aiottech.github.io/iotgateway-docs/<br/>
          <strong>Generated On:</strong> ${dateStr}
        </div>
      </body>
    </html>
  `;

  const page = await browser.newPage();
  await page.setContent(coverHtml, { waitUntil: 'networkidle0' });
  const pdfBuffer = await page.pdf({
    format: 'A4',
    printBackground: true,
    margin: { top: '0mm', bottom: '0mm', left: '0mm', right: '0mm' },
  });
  await page.close();
  return pdfBuffer;
}

async function run() {
  console.log('Waiting for Docusaurus local server...');
  await waitForServer(BASE_URL);
  console.log('Server ready! Launching Chromium...');

  const browser = await puppeteer.launch({
    headless: true,
    args: ['--no-sandbox', '--disable-setuid-sandbox', '--disable-dev-shm-usage'],
  });

  try {
    const pageBuffers = [];
    pageBuffers.push(await generateCoverPdf(browser));

    const page = await browser.newPage();
    await page.setViewport({ width: 1200, height: 900 });

    const visited = new Set();
    let currentUrl = BASE_URL;

    while (currentUrl && !visited.has(currentUrl)) {
      visited.add(currentUrl);
      console.log(`Rendering page: ${currentUrl}`);

      await page.goto(currentUrl, { waitUntil: 'networkidle0' });

      // Wait for all images to decode
      await page.evaluate(async () => {
        const imgs = Array.from(document.images);
        await Promise.all(
          imgs.map((img) =>
            img.complete
              ? Promise.resolve()
              : new Promise((res) => {
                  img.onload = res;
                  img.onerror = res;
                })
          )
        );
      });

      // Inject clean @media print styles with compact datasheet typography
      await page.addStyleTag({
        content: `
          @media print {
            nav.navbar,
            aside.theme-doc-sidebar-container,
            .theme-doc-toc-desktop,
            .theme-doc-toc-mobile,
            .pagination-nav,
            .theme-doc-footer,
            footer.footer,
            .hash-link,
            button[title="Copy code to clipboard"] {
              display: none !important;
            }
            main, .container, .row, .col {
              width: 100% !important;
              max-width: 100% !important;
              flex: 0 0 100% !important;
              margin: 0 !important;
              padding: 0 !important;
            }
            html, body, .markdown {
              font-size: 13px !important;
              line-height: 1.48 !important;
            }
            .markdown h1 {
              font-size: 24px !important;
              margin-bottom: 12px !important;
              break-after: avoid;
            }
            .markdown h2 {
              font-size: 18px !important;
              margin-top: 20px !important;
              margin-bottom: 8px !important;
              break-after: avoid;
            }
            .markdown h3 {
              font-size: 15px !important;
              margin-top: 14px !important;
              margin-bottom: 6px !important;
              break-after: avoid;
            }
            .markdown pre {
              font-size: 10.5px !important;
              line-height: 1.35 !important;
              padding: 10px 12px !important;
              white-space: pre !important;
              overflow-x: visible !important;
              break-inside: avoid;
            }
            .markdown code {
              font-size: 11px !important;
            }
            .markdown table {
              display: table !important;
              width: 100% !important;
              font-size: 11.5px !important;
              border-collapse: collapse !important;
            }
            .markdown thead {
              display: table-header-group !important;
            }
            .markdown th, .markdown td {
              padding: 6px 8px !important;
            }
            .markdown tr, blockquote, img, .admonition {
              break-inside: avoid;
            }
            .theme-doc-breadcrumbs {
              font-size: 11px !important;
              margin-bottom: 8px !important;
            }
          }
        `,
      });

      const pdfBuffer = await page.pdf({
        format: 'A4',
        scale: 0.8,
        printBackground: true,
        margin: { top: '14mm', bottom: '16mm', left: '13mm', right: '13mm' },
      });
      pageBuffers.push(pdfBuffer);

      // Follow the "Next" pagination button in sidebar order
      const nextUrl = await page.evaluate(() => {
        const nextLink = document.querySelector('a.pagination-nav__link--next');
        return nextLink ? nextLink.href : null;
      });

      currentUrl = nextUrl;
    }

    await page.close();

    // Merge all PDF buffers and stamp page numbers onto content pages
    console.log(`Merging ${pageBuffers.length} PDF sections...`);
    const mergedPdf = await PDFDocument.create();
    const font = await mergedPdf.embedFont(StandardFonts.Helvetica);

    for (let idx = 0; idx < pageBuffers.length; idx++) {
      const srcDoc = await PDFDocument.load(pageBuffers[idx]);
      const copiedPages = await mergedPdf.copyPages(srcDoc, srcDoc.getPageIndices());
      copiedPages.forEach((p) => mergedPdf.addPage(p));
    }

    const totalPages = mergedPdf.getPageCount();
    for (let i = 1; i < totalPages; i++) {
      const p = mergedPdf.getPage(i);
      const { width } = p.getSize();
      p.drawText(`CONSTH MGW User Manual  |  https://consth.vn/`, {
        x: 38,
        y: 18,
        size: 8,
        font,
        color: rgb(0.45, 0.5, 0.58),
      });
      const pageLabel = `Page ${i} of ${totalPages - 1}`;
      p.drawText(pageLabel, {
        x: width - 90,
        y: 18,
        size: 8,
        font,
        color: rgb(0.45, 0.5, 0.58),
      });
    }

    fs.mkdirSync(OUTPUT_DIR, { recursive: true });
    const finalBytes = await mergedPdf.save();
    fs.writeFileSync(OUTPUT_PDF, finalBytes);
    console.log(`SUCCESS! Generated PDF manual at: ${OUTPUT_PDF} (${totalPages} pages)`);
  } finally {
    await browser.close();
  }
}

run().catch((err) => {
  console.error('PDF Generation Failed:', err);
  process.exit(1);
});