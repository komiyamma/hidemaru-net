/**
 * CharCanvas.js
 *
 * @description    Draw Tool for Ascii Art.
 * @fileoverview   Character Canvas library
 * @author         Masakazu Yanai
 * @version        0.2.0
 * @date           2017-08-25
 * @link           https://github.com/masakazu-yanai/char-canvas
 * @copyright      Copyright (c) 2017 Masakazu Yanai <yanai@crocro.com>
 * @license        licensed under the MIT license.
 *
 * ///////////////////////////////////////////////////////
 *
 * web              http://crocro.com/
 * start dev        2016-12-06
 */

/**
 * HmCharChanvas.js 数行修正されています。 by vscode-life
 */

(function (name, context, factory) {
	// Supports UMD. AMD, CommonJS/Node.js and browser context
	if (typeof exports !== 'undefined') {
		if (typeof module !== 'undefined' && module.exports) {
			module.exports = factory();
		} else {
			exports[name] = factory();
		}
	} else if (typeof define === 'function' && define.amd) {
		define(factory);
	} else {
		context[name] = factory();
	}
})('CharCanvas', this, function () {
	'use strict';

	var global = Function('return this')();

	// # 文字キャンバスの作成
	// @arg: {
	//    w: '横幅'
	//   ,h: '高さ'
	//   ,c: '埋め文字'
	// }
	var CharCanvas = function(w, h, c) {
		this.w = w;
		this.h = h;
		if (Array.isArray(c)) {
			this.canvas = ' '.repeat(w * h).split('')
				.map((x, i) => c[i % c.length]);
		} else {
			this.canvas = c.repeat(w * h).split('');
		}
	};

	// # 出力 (秀丸用に修正 by vscode-life 2017/10/02)
	CharCanvas.prototype.print = function() {
		hm.Edit.TotalText = this.toString();
	};

	// # 文字列化
	// @return 文字列化した文字キャンバス。
	CharCanvas.prototype.toString = function() {
		var re = new RegExp('.{' + this.w + '}', 'g');
		var t = this.canvas.join('').match(re).join('\n').replace(/\t/g, '');
		return t;
	};

	// # クローン
	// @return クローンした文字キャンバス。
	CharCanvas.prototype.clone = function() {
		var t = this;
		var t2 = new CharCanvas(t.w, t.h, '0');
		t2.canvas = [].concat(t.canvas);
		return t2;
	};

	// # 点シリアライズ
	// @return 直列化した文字の配列
	CharCanvas.prototype.serializePoint = function(arr) {
		var t = this;
		var res = [];
		if (Array.isArray(arr[0])) {
			if (arr.length == 1) {
				res = t.serializePoint(arr[0]);
			} else {
				arr.forEach(x => {
					Array.prototype.push.apply(res, t.serializePoint(x));
				});
			}
		} else
		if (arr[0] && arr[0].x !== undefined && arr[0].y !== undefined) {
			arr.forEach(o => {
				res.push(o.x);
				res.push(o.y);
			});
		} else {
			res = arr;
		}
		return res;
	};

	// # 1マス書く
	// @arg: {
	//    x: 'x座標'
	//   ,y: 'y座標'
	//   ,c: '文字（あるいは文字の配列）'
	// }
	CharCanvas.prototype.set = function(x, y, c) {
		var t = this;
		if (! t.isRng(x, y)) {return}
		t.canvas[x + y * t.w] = Array.isArray(c) ? c[(x + y * t.w) % c.length] : c;
	};

	// # 1マス得る
	// @arg: {
	//    x: 'x座標'
	//   ,y: 'y座標'
	// }
	// @return 指定した座標の文字
	CharCanvas.prototype.get = function(x, y) {
		return this.canvas[x + y * this.w];
	};

	// # 範囲内か
	// @arg: {
	//    x: 'x座標'
	//   ,y: 'y座標'
	// }
	// @return 文字キャンバスの範囲内のboolean。
	CharCanvas.prototype.isRng = function(x, y) {
		if (x < 0 || x >= this.w) {return false}
		if (y < 0 || y >= this.h) {return false}
		return true;
	};

	// # ゼロクローン（バイナリ キャンバスの初期化）
	// @return 同サイズの0埋めした文字キャンバス。
	CharCanvas.prototype.zeroClone = function() {
		var cc = new CharCanvas(this.w, this.h, '0');
		cc.canvas = cc.canvas.map(x => x * 1);
		return cc;
	};

	// # 全マスをスキャン
	// @arg: {
	//    fnc: '走査する関数'
	// }
	// fncの引数は「x, y」。
	CharCanvas.prototype.scan = function(fnc) {
		for (var y = 0; y < this.h; y ++) {
			for (var x = 0; x < this.w; x ++) {
				fnc(x, y);
			}
		}
	};

	// # バイナリ キャンバスの1のところだけをスキャン
	// @arg: {
	//    fnc: '走査する関数'
	// }
	CharCanvas.prototype.scanOne = function(fnc) {
		var t = this;
		t.scan((x, y) => {
			if (t.get(x, y) === 1) {fnc(x, y)}
		});
	};

	// # バイナリ キャンバスの0のところだけをスキャン
	// @arg: {
	//    fnc: '走査する関数'
	// }
	CharCanvas.prototype.scanZero = function(fnc) {
		var t = this;
		t.scan((x, y) => {
			if (t.get(x, y) === 0) {fnc(x, y)}
		});
	};

	// # 周囲4マスをスキャン
	// @arg: {
	//    x: 'X座標'
	//   ,y: 'Y座標'
	//   ,fnc: '走査する関数'
	// }
	// fnc の return が true なら、以降の処理をしない。
	CharCanvas.prototype.scanNear4 = function(x, y, fnc) {
		if (fnc(x - 1, y)) {return}
		if (fnc(x + 1, y)) {return}
		if (fnc(x, y - 1)) {return}
		if (fnc(x, y + 1)) {return}
	};

	//----------------------------------------
	// # エリアをアンド演算 operation and
	// @arg: {
	//    cc: 'バイナリ キャンバス'
	// }
	// 2つのバイナリ キャンバスをAND演算。
	// 自身を背面、引数を前面として計算する。
	CharCanvas.prototype.opAnd = function(cc) {
		var t = this.clone();
		t.scan((x, y) => {
			var tOne  = t.get(x, y)  === 1;
			var ccOne = cc.get(x, y) === 1;
			t.set(x, y, tOne && ccOne ? 1 : 0);
		});
		return t;
	};

	// # 手前のエリアで、奥のエリアを削る operation not
	// @arg: {
	//    cc: 'バイナリ キャンバス'
	// }
	// 2つのバイナリ キャンバスをNOT演算。
	// 自身を背面、引数を前面として計算する。
	CharCanvas.prototype.opNot = function(cc) {
		var t = this.clone();
		t.scan((x, y) => {
			var tOne   = t.get(x, y)  === 1;
			var ccZero = cc.get(x, y) === 0;
			t.set(x, y, tOne && ccZero ? 1 : 0);
		});
		return t;
	};

	// # エリアをオア演算 operation or
	// @arg: {
	//    cc: 'バイナリ キャンバス'
	// }
	// 2つのバイナリ キャンバスをOR演算。
	// 自身を背面、引数を前面として計算する。
	CharCanvas.prototype.opOr = function(cc) {
		var t = this.clone();
		t.scan((x, y) => {
			var tOne  = t.get(x, y)  === 1;
			var ccOne = cc.get(x, y) === 1;
			t.set(x, y, tOne || ccOne ? 1 : 0);
		});
		return t;
	};

	// # エリアをエックスオア演算 operation xor
	// @arg: {
	//    cc: 'バイナリ キャンバス'
	// }
	// 2つのバイナリ キャンバスをXOR演算。
	// 自身を背面、引数を前面として計算する。
	CharCanvas.prototype.opXor = function(cc) {
		var t = this.clone();
		t.scan((x, y) => {
			var tBin  = t.get(x, y);
			var ccBin = cc.get(x, y);
			t.set(x, y, tBin + ccBin === 1 ? 1 : 0);
		});
		return t;
	};

	//----------------------------------------
	// # エリア塗り潰し
	// @arg: {
	//    cc: '1の部分を塗りつぶすバイナリ キャンバス'
	// }
	CharCanvas.prototype.fillArea = function(cc, c) {
		var t = this;
		cc.scanOne((x, y) => {t.set(x, y, c)});
	};

	// # エリア線描画
	// @arg: {
	//    cc: '1の部分が図形に相当するバイナリ キャンバス'
	//   ,c: '文字（あるいは文字の配列）'
	//   ,isFrm: '枠線を付けるか'
	// }
	// isFrm を true にすると、描画領域の外枠に接している線も描画する。
	CharCanvas.prototype.strokeArea = function(cc, c, isFrm) {
		var t = this;
		cc.scanOne((x, y) => {
			t.scanNear4(x, y, (x2, y2) => {
				if (! isFrm && (! cc.isRng(x2, y2) || cc.get(x2, y2) === 1)) {return}
				if (  isFrm && (  cc.isRng(x2, y2) && cc.get(x2, y2) === 1)) {return}
				t.set(x, y, c);
				return true;
			});
		});
	};

	// # 関数によるエリア塗り潰し
	// @arg: {
	//    fncArea : '文字列でエリアの種類関数（areaCircle, areaRect など）を指定'
	//   ,c : '文字（あるいは文字の配列）'
	//   ,argOther : '引数2以降は、fncArea に渡す引数。'
	// }
	CharCanvas.prototype.fillFnc = function(fncArea, c) {
		var t = this, arg = arguments;
		var argPipe = [];
		for (var i = 2; i < arg.length; i ++) {argPipe.push(arg[i])}
		var cc = this[fncArea].apply(t, argPipe);
		t.fillArea(cc, c);
	};

	//----------------------------------------
	// # エリア 円
	// @arg: {
	//    cx : '中心X座標'
	//   ,cy : '中心Y座標'
	//   ,rx : '半径X座標'
	//   ,rx : '半径X座標'
	// }
	CharCanvas.prototype.areaCircle = function(cx, cy, rx, ry) {
		var cc = this.zeroClone();
		cc.scan((x, y) => {
			if ((x-cx)*(x-cx)/(rx*rx) + (y-cy)*(y-cy)/(ry*ry) < 1) {
				cc.set(x, y, 1);
			}
		});
		return cc;
	};

	// # エリア 四角形
	// @arg: {
	//    bx : '左上X座標'
	//   ,by : '左上Y座標'
	//   ,w : '横幅'
	//   ,h : '高さ'
	// }
	CharCanvas.prototype.areaRect = function(bx, by, w, h) {
		var cc = this.zeroClone();
		cc.scan(function(x, y) {
			if (bx <= x && x < bx + w && by <= y && y < by + h) {
				cc.set(x, y, 1);
			}
		});
		return cc;
	};

	// # エリア パス
	// @arg: {
	//    arg : '引数2つずつが、X座標、Y座標に相当'
	// }
	// X座標0、Y座標0、X座標1、Y座標1、X座標2、Y座標2……のように座標を指定。
	// ver 0.2.0以降。以下の形式でも可能。
	// 配列　[X座標0、Y座標0、X座標1、Y座標1、X座標2、Y座標2……]
	// 点の配列1　[[X座標0、Y座標0]、[X座標1、Y座標1]……]
	// 点の配列2　[{x:X座標0、y:Y座標0}、{x:X座標1、y:Y座標1}……]
	CharCanvas.prototype.areaPath = function() {
		var cc, arg = this.serializePoint(arguments);

		for (var i = 0; i < arg.length; i += 2) {
			if (i == 0) {
				cc = this.moveTo(arg[i], arg[i + 1]);
			} else {
				cc.lineTo(arg[i], arg[i + 1]);
			}
		}
		return cc;
	};

	//----------------------------------------
	// # パス基点移動
	// @arg: {
	//    x : '基点X座標'
	//   ,y : '基点Y座標'
	// }
	CharCanvas.prototype.moveTo = function(x, y) {
		var cc = this.zeroClone();
		cc.pntStrt =  {x: x, y: y};
		cc.pntNow  =  {x: x, y: y};
		cc.pntArr  = [{x: x, y: y}];
		return cc;
	};

	// # 現在点からパスを引く
	// @arg: {
	//    x : '移動X座標'
	//   ,y : '移動Y座標'
	// }
	CharCanvas.prototype.lineTo = function(x, y) {
		var t = this;
		this.line(t.pntNow.x, t.pntNow.y, x, y, 1);
		t.pntNow.x = x;
		t.pntNow.y = y;
		t.pntArr.push({x: x, y: y});
		return t;
	};

	// # パスを閉じる
	CharCanvas.prototype.close = function() {
		var t = this;
		t.line(t.pntNow.x, t.pntNow.y, t.pntStrt.x, t.pntStrt.y, 1);
		t.pntNow.x = t.pntStrt.x;
		t.pntNow.y = t.pntStrt.y;
		return t;
	};

	// # 線を引く
	// @arg: {
	//    x0 : 'X座標0'
	//   ,y0 : 'Y座標0'
	//   ,x1 : 'X座標1'
	//   ,y1 : 'Y座標1'
	//   ,c : '文字（あるいは文字の配列）'
	// }
	// ブレゼンハム アルゴリズムで線を引く。
	CharCanvas.prototype.line = function(x0, y0, x1, y1, c) {
		var dx = Math.abs(x1 - x0);
		var dy = Math.abs(y1 - y0);
		var sx = (x0 < x1) ? 1 : -1;
		var sy = (y0 < y1) ? 1 : -1;
		var err = dx - dy;

		this.set(x0, y0, c);
		while (! (x0 == x1 && y0 == y1)) {
			var e2 = err << 1;
			if (e2 > -dy) {err -= dy; x0 += sx}
			if (e2 <  dx) {err += dx; y0 += sy}
			this.set(x0, y0, c);
		}
	};

	// # パスの中を塗り潰し
	// moveto lineto で引いた線の中を塗り潰す。
	CharCanvas.prototype.fillPath = function() {
		var t = this;

		t.scan((x, y) => {
			var inside = false;
			var p1, p2;
			for (var i = 0; i < t.pntArr.length; i ++) {
				var pt1 = t.pntArr[i];
				var pt2 = t.pntArr[i > 0 ? i - 1 : t.pntArr.length - 1];

				if (pt1.x > pt2.x) {p1 = pt2; p2 = pt1}
				else               {p1 = pt1; p2 = pt2}

				if ((p1.x < x) == (x <= p2.x)
				 && (y - p1.y) * (p2.x - p1.x) < (p2.y - p1.y) * (x - p1.x)
				) {
					inside = !inside;
				}
			}
			if (inside) {t.set(x, y, 1)}
		});

		return t;
	};

	//----------------------------------------
	// # 文字列の描画
	// @arg: {
	//    x : 'X座標'
	//   ,y : 'Y座標'
	//   ,txt : '文字列'
	// }
	// 文字列は、基本的には半角文字のみ。
	// 全角文字を使いたい場合は、背景も全角文字にする。
	CharCanvas.prototype.drawText = function(x, y, txt) {
		var t = this;
		txt.split('\n').forEach((line, y2) => {
			line.split('').forEach((c, x2) => {
				t.set(x + x2, y + y2, c);
			});
		});
	};

	// # 文字列の描画 全角対応
	// @arg: {
	//    x : 'X座標'
	//   ,y : 'Y座標'
	//   ,txt : '文字列'
	// }
	// 半角の文字キャンバスに、全角混じりの文字を描画する際に使う。
	CharCanvas.prototype.drawTextZ = function(x, y, txt) {
		var t = this;
		txt.split('\n').forEach((line, y2) => {
			var x2 = 0;
			line.split('').forEach(c => {
				if (c.match(/[^\x01-\x7E\xA1-\xDF]/)) {
					t.set(x + x2, y + y2, c);
					x2 ++;
					t.set(x + x2, y + y2, '\t');
				} else {
					t.set(x + x2, y + y2, c);
				}
				x2 ++;
			});
		});
	};

	// # 文字の形で塗り潰し
	// @arg: {
	//    x : 'X座標'
	//   ,y : 'Y座標'
	//   ,txtLine : '文字列、1行のみを想定'
	//   ,fontSizePx : 'フォントのサイズ、単位はピクセル'
	//   ,fontStyle : 'Canvasに描画するフォントのスタイル'
	//	 ,drawRate: '色の閾値の数値、1が濃く0になるほど薄くなる'
	//   ,wRate: '横幅比率'
	// }
	// fontStyle は、書き方によって実際に適用されるスタイルが異なります。
	// 
	// ・フォント名のみ書いた場合→「fontSizePx + 'px ' + fontStyle」
	// ・「bold 16px sans-serif」のように指定した場合→「fontStyle」がそのまま適用
	// 
	// 正規表現 /\dpx / に一致するかで判断しています。
	CharCanvas.prototype.areaText = function(x, y, txtLine,
	fontSizePx, fontStyle, drawRate, wRate) {

        // 秀丸用にエラー追加。(秀丸用に修正 by vscode-life 2017/10/02)
		hm.Macro.Eval(`message "areaText関数は、ブラウザのcanvas用ですので、秀丸用途では機能しません";`);
		return;

		// 変数の初期化
		var cc = this.zeroClone();
		if (! global.document) {return cc}

		// キャンバスの作成と文字列の描画
		var cnvs = document.createElement('canvas');
		var w = fontSizePx * txtLine.length * 1.5 | 0;
		var h = fontSizePx * 1.5 | 0;
		cnvs.width  = w;
		cnvs.height = h;
		cnvs.style.display = 'none';
		document.body.appendChild(cnvs);
		var cntxt = cnvs.getContext('2d');
		cntxt.fillStyle = '#fff';
		cntxt.fillRect(0, 0, w, h);

		cntxt.fillStyle = '#000';
		if (! fontStyle.match(/\dpx /)) {
			cntxt.font = fontSizePx + 'px ' + fontStyle;
		} else {
			cntxt.font = fontStyle;
		}
		cntxt.textAlign = 'left';
		cntxt.textBaseline = 'top';
		cntxt.fillText(txtLine, 0, 0);

		// 変数の初期化
		var dt = cntxt.getImageData(0, 0, w, h).data;
		var threshold = 255 * drawRate | 0;
		threshold = Math.max(0, Math.min(threshold, 255));

		// RGB要素の取得
		for (var y2 = 0; y2 < h; y2 ++) {
			var i = -1;
			for (var x2 = 0; x2 < w; x2 ++) {
				var pos = (y2 * w + x2) * 4;
				if (dt[pos] >= threshold) {continue}

				for (i = (x2 - 1) * wRate | 0; i < x2 * wRate; i ++) {
					cc.set(x + i, y + y2, 1);
				}
			}
		}
		return cc;
	};

	// 本体を戻す
	return CharCanvas;
});
