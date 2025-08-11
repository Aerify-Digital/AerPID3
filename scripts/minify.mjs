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
const fileTree = {
  index: 'index.html',
  js: ['js/aertiny.js', 'js/charting.js']
};

const minifyChecksumTree = async () => {
  const dir = loc + dataLoc;
  // Load in your code to minify
  const parContent = readFileSync(`${dir}${fileTree.index}`, 'utf8');
  let contentOut = parContent;
  // minify and checksum the files
  for (const file of fileTree.js) {
    const { fileSource, checksumName } = await minifyFile(file);
    contentOut = contentOut.replace(fileSource, checksumName);
  }
  const fSplit = fileTree.index.split('.');
  const ext = `.${fSplit[fSplit.length - 1]}`;
  const file = fileTree.index.replace(ext, '');
  const fileOut = dir + file + '.min' + ext;
  // minify the root file
  const minifiedOut = await minifyHtml(contentOut, htmlConfig);
  // Save the code!
  writeFileSync(fileOut, minifiedOut);
};

const minifyFile = async (fn) => {
  const dir = loc + dataLoc;
  const fSplit = fn.split('.');
  const ext = `.${fSplit[fSplit.length - 1]}`;
  const file = fn.replace(ext, '');
  const fileIn = dir + file + ext;
  // Load in your code to minify
  const code = readFileSync(fileIn, 'utf8');
  let minified;
  if (ext === '.js') {
    // Minify the code with Terser
    minified = (await minify(code, jsConfig)).code;
  } else if (ext === '.html') {
    // Minify the code with Terser for html
    minified = await minifyHtml(code, htmlConfig);
  } else {
    return;
  }
  // checksum the file content
  const checksum = crc32(minified);
  const checksumName = `${file}.min-${checksum}${ext}`;
  const fileSource = file + '.min' + ext;
  const checksumFile = dir + checksumName;
  // Save the code!
  writeFileSync(checksumFile, minified);
  console.log('>> Minify Processed: ' + fn);
  return { fileSource, checksumName };
};

const makeCRCTable = function () {
  var c;
  var crcTable = [];
  for (var n = 0; n < 256; n++) {
    c = n;
    for (var k = 0; k < 8; k++) {
      c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    }
    crcTable[n] = c;
  }
  return crcTable;
};

const crc32 = function (str) {
  var crcTable = crcTable || (crcTable = makeCRCTable());
  var crc = 0 ^ -1;
  for (var i = 0; i < str.length; i++) {
    crc = (crc >>> 8) ^ crcTable[(crc ^ str.charCodeAt(i)) & 0xff];
  }
  return (crc ^ -1) >>> 0;
};

const execute = () => {
  return new Promise(async (resolve, reject) => {
    await minifyChecksumTree();
    console.log('> Minify Complette');
    resolve();
  });
};

execute();
