# Copyright (c) 2017 Akitsugu Komiyama
# under the MIT License
# 
# 要 panflute : (python -m pip install panfulate)

from panflute import *

import sys

def increase_header_level(elem, doc):
    if type(elem) == Math:
        mathtext = elem.text
        if elem.format == "DisplayMath":
            # To-Do 実際にはここで「HmMathjaxToSvg.node.js」でイメージを生成
            return RawInline('<img src = "display.png">', format='html')

        if elem.format == "InlineMath":
            # To-Do 実際にはここで「HmMathjaxToSvg.node.js」でイメージを生成
            return RawInline('<img src = "inline.png">', format='html')
            
    return elem

def main(doc=None):
    return run_filter(increase_header_level, doc=doc)

if __name__ == "__main__":
    main()