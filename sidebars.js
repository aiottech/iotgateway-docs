/** @type {import('@docusaurus/plugin-content-docs').SidebarsConfig} */
const sidebars = {
  tutorialSidebar: [
    {
      type: 'category',
      label: '1. Overview & Setup',
      items: [
        '01_overview/introduction',
        '01_overview/quick_start',
        '01_overview/hardware_pinout',
      ],
    },
    {
      type: 'category',
      label: '2. Communication Protocols',
      items: [
        '02_communication_protocols/data_transfer_modes',
        '02_communication_protocols/uart_setup',
        '02_communication_protocols/spi_high_speed',
      ],
    },
    {
      type: 'category',
      label: '3. Host MCU Integration',
      items: [
        '03_host_mcu_integration/overview',
        '03_host_mcu_integration/file_explorer_bridge',
        '03_host_mcu_integration/web_plugins_bridge',
      ],
    },
    {
      type: 'category',
      label: '4. Cloud & Advanced Features',
      items: [
        '04_cloud_and_advanced_features/security_and_auth',
        '04_cloud_and_advanced_features/signalr_integration',
        '04_cloud_and_advanced_features/ota_firmware_updates',
        '04_cloud_and_advanced_features/native_ftp_server',
      ],
    },
    {
      type: 'category',
      label: '5. Reference Manuals',
      items: [
        '05_reference/command_reference',
        '05_reference/error_codes',
      ],
    },
  ],
};

export default sidebars;