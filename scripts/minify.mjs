// Import Terser so we can use it
import { minify } from 'terser';
import { minify as minifyHtml } from 'html-minifier-terser';

// Import fs so we can read/write files
import { readFileSync, writeFileSync } from 'fs';

// Define the config for how Terser should minify the code
const jsConfig = {
  compress: {
    dead_code: true,
    drop_console: true,
    drop_debugger: true,
    keep_classnames: false,
    keep_fargs: true,
    keep_fnames: false,
    keep_infinity: false
  },
  mangle: {
    eval: false,
    keep_classnames: false,
    keep_fnames: false,
    toplevel: false,
    safari10: false
  },
  module: false,
  sourceMap: false,
  output: {
    comments: 'some'
  }
};

const htmlConfig = {
  removeAttributeQuotes: true,
  minifyJS: true,
  minifyCSS: true,
  removeComments: true,
  collapseWhitespace: true,
  removeRedundantAttributes: true,
  trimCustomFragments: true
};

const loc = process.argv[2];
const dataLoc = 'data/';
const files = ['js/aertiny.js', 'js/charting.js', 'index.html'];

for (const fn of files) {
  const dir = loc + dataLoc;
  const fSplit = fn.split('.');
  const ext = `.${fSplit[fSplit.length - 1]}`;
  const file = fn.replace(ext, '');
  const fileIn = dir + file + ext;
  const fileOut = dir + file + '.min' + ext;

  // Load in your code to minify
  const code = readFileSync(fileIn, 'utf8');

  let minified;
  if (ext == '.js') {
    // Minify the code with Terser
    minified = (await minify(code, jsConfig)).code;
  } else if (ext == '.html') {
    // Minify the code with Terser for html
    minified = await minifyHtml(code, htmlConfig);
  } else {
    continue;
  }

  // Save the code!
  writeFileSync(fileOut, minified);

  console.log('>> Minify Processed: ' + fn);
}
