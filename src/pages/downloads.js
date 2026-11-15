import React from 'react';
import Layout from '@theme/Layout';
import useBaseUrl from '@docusaurus/useBaseUrl';

export default function Downloads() {
  const sdkZipUrl = useBaseUrl('/sdk/mgw_host_mcu_sdk_v1.0.zip');

  return (
    <Layout
      title="SDK & Documentation Downloads"
      description="Download the official ANSI C Host MCU Drivers and PDF User Manuals for the CONSTH MGW-IoT-002 Gateway."
    >
      <main className="container margin-vert--lg">
        <div className="row">
          <div className="col col--10 col--offset-1">
            <h1>Downloads &amp; Resources</h1>
            <p className="margin-bottom--lg">
              Download the official Host Microcontroller C SDK drivers and documentation packages for the{' '}
              <strong>CONSTH MGW-IoT-001 / MGW-IoT-002</strong> gateway series.
            </p>

            <div className="row">
              {/* Card 1: Host MCU C SDK */}
              <div className="col col--6 margin-bottom--lg">
                <div className="card shadow--md" style={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
                  <div className="card__header">
                    <h3>📦 Host MCU C SDK (v1.0)</h3>
                  </div>
                  <div className="card__body" style={{ flex: 1 }}>
                    <p>
                      Pure, portable ANSI C drivers with zero dynamic memory allocations (<code>malloc</code>-free).
                      Compatible with STM32, NXP, Microchip, PIC, and AVR microcontrollers.
                    </p>
                    <ul>
                      <li><code>mgw_host_mcu_parser.h / .c</code> (Stream V2 &amp; Normal Mode FSM)</li>
                      <li><code>mgw_host_file_bridge.h / .c</code> (File Explorer &amp; Web Plugin Bridge)</li>
                    </ul>
                  </div>
                  <div className="card__footer">
                    <a
                      className="button button--primary button--block margin-bottom--sm"
                      href={sdkZipUrl}
                      download="mgw_host_mcu_sdk_v1.0.zip"
                    >
                      ⬇ Download C SDK (.zip)
                    </a>
                    <a
                      className="button button--secondary button--outline button--block"
                      href="https://github.com/aiottech/iotgateway-docs/tree/main/sdk/c_driver"
                      target="_blank"
                      rel="noopener noreferrer"
                    >
                      View Source on GitHub ↗
                    </a>
                  </div>
                </div>
              </div>

              {/* Card 2: Offline PDF User Manual (Ready for Task 2!) */}
              <div className="col col--6 margin-bottom--lg">
                <div className="card shadow--md" style={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
                  <div className="card__header">
                    <h3>📄 Offline PDF User Manual</h3>
                  </div>
                  <div className="card__body" style={{ flex: 1 }}>
                    <p>
                      Complete, printable offline technical manual compiled directly from this developer portal for
                      air-gapped industrial environments and engineering archives.
                    </p>
                    <ul>
                      <li>Quick Start &amp; Hardware Pinout</li>
                      <li>UART &amp; High-Speed SPI Integration</li>
                      <li>Full CLI Command &amp; Error Code Reference</li>
                    </ul>
                  </div>
                  <div className="card__footer">
                    <a
                      className="button button--secondary button--outline button--block"
                      href="https://github.com/aiottech/iotgateway-docs/releases"
                      target="_blank"
                      rel="noopener noreferrer"
                    >
                      View PDF Releases on GitHub ↗
                    </a>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </div>
      </main>
    </Layout>
  );
}