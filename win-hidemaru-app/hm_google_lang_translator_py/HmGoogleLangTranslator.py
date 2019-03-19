# -*- coding: utf-8 -*-

#
# HmGoogleLangTranslator.mac v1.0.0.1
# Copyright (c) 2019 Akitsugu Komiyama
# under the MIT License
#
# 秀丸 v8.66以上
# 要 hmPython3 1.5.5.1 以上
#

from hmPython import hm
import json
import urllib.request

def TranslateByGet(text, source, target):

    script_url = hm.Macro.Var["$url"]

    params = {
        "text": text,
        "source": source,
        "target": target
    }
    encode_params = urllib.parse.urlencode(params)
    request_script_url = '{}?{}'.format(script_url, encode_params)

    req = urllib.request.Request(request_script_url)
    with urllib.request.urlopen(req) as res:
        byte_result = res.read()
        answer = byte_result.decode(encoding='utf-8')
        return answer

    return ""


#-------------------------------------------------------------
# 上のGetリクエストに対応できるように、
# Google Application Script 側はサーバー側で以下のように定義
#-------------------------------------------------------------
# Getメソッド用
# function doGet(e) {
#     var p = e.parameter;
#     var translatedText = LanguageApp.translate(p.text, p.source, p.target);
#     return ContentService.createTextOutput(translatedText);
# }


def TranslateByPost(text, source, target):
    headers = {"Content-Type": "application/json"}

    script_url = hm.Macro.Var["$url"]

    data = {
        "text": text,
        "source": source,
        "target": target
    }
    json_data = json.dumps(data).encode("utf-8")

    req = urllib.request.Request(script_url, json_data, method="POST", headers=headers)
    with urllib.request.urlopen(req) as res:
        byte_result = res.read()
        response_body = byte_result.decode(encoding='utf-8')
        response_json = json.loads(response_body)
        return response_json["result"]

    return "";


#-------------------------------------------------------------
# 上のPostリクエストに対応できるように、
# Google Application Script 側はサーバー側で以下のように定義
#-------------------------------------------------------------
# Postメソッド用
# function doPost(e) {
#     var p = JSON.parse(e.postData.getDataAsString());
#     var translatedText = LanguageApp.translate(p.text, p.source, p.target);
#
#     var output = ContentService.createTextOutput();
#     output.setMimeType(ContentService.MimeType.JSON);
#     output.setContent(JSON.stringify({ result: translatedText }));
#
#     return output;
# }

