// @ts-check
import {themes as prismThemes} from 'prism-react-renderer';

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'CONSTH MGW Developer Center',
  tagline: 'Official Technical Documentation for MGW Gateways by CONSTH Vietnam',
  favicon: 'img/logo.png',

  url: 'https://aiottech.github.io',
  baseUrl: '/iotgateway-docs/',

  organizationName: 'aiottech', 
  projectName: 'iotgateway-docs', 

  onBrokenLinks: 'warn',
  markdown: {
    hooks: {
      onBrokenMarkdownLinks: 'warn',
    },
  },

  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      /** @type {import('@docusaurus/preset-classic').Options} */
      ({
        docs: {
          sidebarPath: require.resolve('./sidebars.js'),
          routeBasePath: '/',
        },
        blog: false,
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
        },
      }),
    ],
  ],

  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      navbar: {
        title: 'MGW Docs',
        logo: {
          alt: 'CONSTH Vietnam Logo',
          src: 'img/logo.png',
          href: 'https://consth.vn/', // Clicking the logo opens the official CONSTH website
        },
        items: [
          {
            type: 'docSidebar',
            sidebarId: 'tutorialSidebar',
            position: 'left',
            label: 'Gateway Manual',
          },
          {
            href: 'https://consth.vn/',
            label: 'CONSTH Official Website ↗',
            position: 'right',
          },
          {
            href: 'https://github.com/aiottech/iotgateway',
            label: 'GitHub Firmware',
            position: 'right',
          },
        ],
      },
      footer: {
        style: 'dark',
        links: [
          {
            title: 'Documentation',
            items: [
              {
                label: 'Introduction',
                to: '/',
              },
              {
                label: 'Quick Start Guide',
                to: '/overview/quick_start',
              },
              {
                label: 'Hardware Pinout',
                to: '/overview/hardware_pinout',
              },
            ],
          },
          {
            title: 'Company & Distribution',
            items: [
              {
                label: 'Công ty CP Thương Mại CONSTH',
                href: 'https://consth.vn/',
              },
              {
                label: 'CONSTH Corporate Portal',
                href: 'https://consth.vn/',
              },
            ],
          },
          {
            title: 'Software & Repositories',
            items: [
              {
                label: 'Firmware Releases',
                href: 'https://github.com/aiottech/iotgateway/tree/main/bin',
              },
              {
                label: 'GitHub Organization',
                href: 'https://github.com/aiottech',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} Công ty Cổ phần Thương mại CONSTH (CONSTH Vietnam). All rights reserved.`,
      },
      prism: {
        theme: prismThemes.github,
        darkTheme: prismThemes.dracula,
        additionalLanguages: ['c', 'cpp', 'json', 'bash'],
      },
    }),
};

export default config;