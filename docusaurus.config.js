// @ts-check
import {themes as prismThemes} from 'prism-react-renderer';

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'AIoT Tech Developer Center',
  tagline: 'Official Technical Documentation for MGW Gateways & Edge Devices',
  favicon: 'img/favicon.ico',

  url: 'https://aiottech.github.io',
  baseUrl: '/iotgateway-docs/',

  organizationName: 'aiottech', 
  projectName: 'iotgateway-docs', 

  onBrokenLinks: 'warn',
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
        title: 'AIoT Tech Docs',
        logo: {
          alt: 'AIoT Tech Logo',
          src: 'img/logo.svg',
        },
        items: [
          {
            type: 'docSidebar',
            sidebarId: 'tutorialSidebar',
            position: 'left',
            label: 'MGW Gateway Manual',
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
        copyright: `Copyright © ${new Date().getFullYear()} AIoT Tech Inc. All rights reserved.`,
      },
      prism: {
        theme: prismThemes.github,
        darkTheme: prismThemes.dracula,
        additionalLanguages: ['c', 'cpp', 'json', 'bash'],
      },
    }),
};

export default config;