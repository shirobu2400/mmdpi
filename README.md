# mmdpi
Miku_Miku_Pi(mmdpi)v0.1 

shirobu(または wq2400kskm)と申します。<br>
限られた処理系(Raspberry Pi 等)でも <br>
MMD っぽいことを実現のためにライブラリを作りました。<br>
PMD,PMXまた、VMDに対応しています。<br>
現在は表示、再生のみしか行えません。<br>
バグがたくさんありますのでライブラリとして使用するには頼りないですが、<br>
このプログラム、ライブラリでコンピュータグラフィックスに興味を持って頂ける方が増えたら私は嬉しいです。<br>
なお、本プログラムはpng, jpegファイル読み込みにlibpng, libjpegをしようさせていただいてます。<br>
libpng, libjpeg がない場合には zlib, libpng, libjpeg をインストールしてください。<br>


# ディレクトリの構成

<ul>
  <li>
  LICENSE.md    :   このソースコードのライセンス
  </li><li>
  README.md    :   このファイル
  </li><li>
  libmmdpi    :   ライブラリフォルダ( mmdpi の処理 )  .cpp, .h があります。
  </li><li>
  mmdpiv       :   ビューアフォルダ                    .cpp, .h があります。
  </li>
</ul>


# コンパイル方法(Raspbian)
<p style="padding-left:2em">
  cmake -Dpi=on <br>
  make  <br>
</p>
cmake を -Dpi=on オプションで実行してください。<br>

# コンパイル方法(Other OS)(確かめていないOSあり。)
<p style="padding-left:2em">
  cmake .　<br>
  make  <br>
</p>
bullet オプションで Bullet Physics インストール済みの場合に物理演算が可能です。<br>

make で ./mmdpiv に実行ファイル mmdpiv ができます。<br>
実行ファイル mmdpiv は　ビューアプログラムです。<br>
GL, GLU, GLUT, GLEWが必要です。<br>
Bullet Physics インストール済みおよび、-Dbullet=on オプションを付けるとBullet Physics による物理演算を行えます。<br>
-Doutline=onをすると輪郭が表示されます。<br>
linux x64 では-Dx64=onオプションを付けてください。<br>
(ただし、正常に動作するかは一部を除いて未確認。)<br>

# 実行方法
./mmdpiv -p[モデルファイルパス] -v[VMD ファイルパス]

# その他、注意事項など

  Raspberry Piの Raspbian 上での動作を想定にプログラムされています。<br>
  VRAM は128MB以上推奨。<br>
  OpenGL 2.0 以上で動作ができます。（頂点テクスチャを使用していないようになっています。）<br>
  一部モデルが正常に表示されない、IKが動作しない問題があります。<br>
  このプログラムで起きた問題等には責任を負いかねます。 <br>
  
  
# 動作しない場合
  
  Q.  何も画面に表示されない場合。<br>
  A.  VRAM が足りない場合があります。VRAM を増やして実行をしてみてください。<br>
      OSがRaspbianではないときはドライバを変更してみてください。 
      <br>
   
# 余談  
  
  Raspbian で OpenGL ES を扱う方へ<br>
  
  Raspbian には頂点テクスチャが実装されてないのでボーンの割り当てに苦労しました。<br>
  <p>  =>  メッシュ分割で解決しました。GPUに渡せる行列は24が限界？</p><br>
  
  Raspbian の頂点インデックスは unsigned int 以上の大きさの数値は扱えない。<br>
  <p> =>  unsigned short 型に収まるようにメッシュ分割しました。</p><br>
  
  
# ご質問、ご指摘、ご要望などがありましたら気軽にご連絡ください。
  
  Twitter : @2400kskm<br>
  ニコニコ: shirobu<br>
