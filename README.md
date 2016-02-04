Raspbian 上での動作を想定しています。

# mmdpi
Miku_Miku_Pi(mmdpi)v0.1 

shirobu(または wq2400kskm)と申します。<br>
限られた処理系(Raspberry Pi 等)でも MMD をやってみたくて、それを出来るためのライブラリを作りました。<br>
バグがたくさんありますのでライブラリとして使用するには頼りないですが、<br>
このプログラム、ライブラリでコンピュータグラフィックスに触れられる人が増えたら私は嬉しいです。<br>
なお、本プログラムはpngファイル読み込みにlibpngをしようさせていただいてます。<br>
libpng がない場合には zlib, libpng をインストールしてください。<br>


# ディレクトリの構成

<ul>
  <li>
  COPYING     :   このソースコードのライセンス
  </li><li>
  libmmdpi    :   ライブラリフォルダ( mmdpi の処理 )  .cpp, .h があります。
  </ul>
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

# コンパイル方法(Other OS)
<p style="padding-left:2em">
  cmake .　<br>
  make  <br>
</p>
bullet オプションで Bullet Physics インストール済みの場合に物理演算が可能です。<br>

make で ./mmdpiv に実行ファイル mmdpiv ができます。<br>
実行ファイル mmdpiv は　ビューアプログラムです。<br>


# その他、注意事項など

  Raspberry Piの Raspbian 上での動作を想定にプログラムされています。<br>
  VRAM は128MB以上推奨。<br>
  OpenGL 2.0 以上で動作ができます。（頂点テクスチャを使用していないようになっています。）<br>
  一部モデルが正常に表示されない、IKが動作しない問題があります。
  
  
# 動作しない場合
  
  Q.  何も画面に表示されない場合。<br>
  A.  VRAM が足りない場合があります。VRAM を増やして実行をしてみてください。<br>

  
# 余談  
  
  Raspbian で OpenGL ES を扱う方へ<br>
  
  Raspbian には頂点テクスチャが実装されてないのでボーンの割り当てに苦労しました。<br>
  <p>  =>  メッシュ分割で解決しました。GPUに渡せる行列は24が限界？</p><br>
  
  Raspbian の頂点インデックスは unsigned int 以上の大きさの数値は扱えない。<br>
  <p> =>  unsigned short 型に収まるようにメッシュ分割しました。</p><br>
  
  
# 質問などがありましたら気軽にご連絡ください。
  
  Twitter : @2400kskm
  