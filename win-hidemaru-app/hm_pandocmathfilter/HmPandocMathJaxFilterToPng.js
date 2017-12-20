/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 *
 * 要 mathjax-node : (npm install mathjax-node)
 * 要 magick.exe : (ImageMagick for Windows)
 */


// ファイル出力用
const fs = require("fs"); 
const mjAPI = require("mathjax-node");

hash_filebase = process.argv[2] // １番目はnode.exe ２番めがこの.jsファイル自身の名前 3番めがハッシュ
hash_filedir   = process.argv[3] // 2番目は出力すべきフォルダ

mjAPI.config(
    {
        MathJax:
        {
            // traditional MathJax configuration
        }
    }
);
mjAPI.start();


var yourMath = fs.readFileSync(hash_filedir + "\\" + hash_filebase + ".txt", "utf-8");

const execSync = require('child_process').exec;

mjAPI.typeset
(
    {
        math: yourMath,
        format: "TeX", // "inline-TeX", "MathML"
        svg: true,
    },
    function (data) {
        if (!data.errors) {
            // console.log(data.svg);

            // ファイル出力
            fs.writeFileSync(hash_filedir + "\\" + hash_filebase + ".svg", data.svg);

            // 通常ブラウザで表示する際の2倍ぐらいで出る
            const result = execSync("magick -density 200 " + hash_filedir + "\\" + hash_filebase +".svg " + hash_filedir + "\\" + hash_filebase + ".png")

        }
    }
);

