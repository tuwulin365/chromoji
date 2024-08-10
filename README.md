# Chromoji

Chromoji is a chrome extension displaying emojis.

You can download it in the [Chrome App Store](https://chrome.google.com/webstore/detail/chromoji/negakbijaemdgbhklopmghphgaeadmpo?hl=fr)

# Libraries

* [js-emoji](https://github.com/iamcal/js-emoji)
* [twemoji](https://github.com/twitter/twemoji)
* [emojione](https://github.com/Ranks/emojione)

# Known issues

Some emojis are not displayed, either because they are not defined in js-emoji or because the images are not available
in the icons packs.



# 我的说明

作者好久没更新了，我来整一整。主要是更新下图片资源，其他我也不会。:smile_cat:



想直接使用的看这：

直接下载插件安装包chromoji.crx，安装使用。插件的离线安装方法请自行百度。



想自己更新的看这：

图片资源在这里下

https://github.com/iamcal/emoji-data

我已经放在 /pic 里了。有两种，high是24位图，高清，但是体积大；low是8位索引图，画质差点，但是体积小。默认用的是low，可以根据需要自己替换，路径是 

/chromoji/data/images/emoji/emoji-data

我还整了个小工具，用来生成索引表。图片和索引表必须版本一致才能正常显示。

先下载

https://github.com/iamcal/emoji-data/blob/master/emoji_pretty.json 或者

https://github.com/iamcal/emoji-data/blob/master/emoji.json

放到 /chromoji/tools/json_tool/bin/release，

根据图片分辨率修改批处理cjson_test.bat，然后执行批处理，即可生成索引表。把表内容复制替换下面这个文件里的数据即可：

/chromoji/data/script/emoji.min.js

build.bat是打包脚本，里面的路径请根据实际情况进行修改。



