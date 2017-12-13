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


mjAPI.config(
    {
        MathJax:
        {
            // traditional MathJax configuration
        }
    }
);
mjAPI.start();


var yourMath = fs.readFileSync("math.txt", "utf-8");

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
            fs.writeFileSync("math.svg", data.svg);

            // 通常ブラウザで表示する際の2倍ぐらいで出る
            const result = execSync("magick -density 200 math.svg math.png")

        }
    }
);

