# DS4vJoy
DualShock 4 to vJoy for Windows

![log](doc/ssLog.png)
## これなに
仮想ジョイスティックのvJoyを経由することでDS4を簡単に使えます。  

- 日本語対応！
- 振動簡易対応！
- 連射対応！
- タッチパッド分割対応！
- キーボード入力対応！(SendInput)
- Bluetooth対応！

## つかいかた
1. [vJoy 2.1.6](https://sourceforge.net/projects/vjoystick/)をインストールします。  
2. Configure vJoyを開いてButtonsを128にPOVを1に選択してください。  
Axes、Enable Effectsなども全てチェックを入れておいてください。PC再起動になるかもしれません。  
3. DS4vJoy.exeを起動します。  
4. PCとDS4をUSBやBluetoothで接続します。  
5. ゲームなどから、ゲームパッドとしてvJoyを選ぶと、DS4の操作が反映されます。

## メリット
- ゲーム起動後にDS4を接続しても大丈夫です。 ゲームにはvJoyが常に見えています。
- タッチパッドを2列2行に分割すると、４つのボタンを割り当てられます。  
![TouchPad](doc/ds4Touchpad.png)  
Monitor vJoyを起動すれば、どのボタンが押されているか確認できます。
- Mapping DS4のボタンとvJoyのボタンは自由に関連付けできます。128ボタンまでDS4からの入力が複数作れます。
- RapidFire 連射機能が使えます。
- Keymap ゲームで使わないボタンをキー入力として活用できます。

## デメリット
- PCにvJoyをインストールしないと使えません。
- 振動機能がしょぼいです。

## スクリーンショット
- ![Setting](doc/ssSetting.png)
- タッチパッドの左側をクリックするとボタン20、右側はボタン21が有効になります。  
Serialを指定すると、たぶんDS4が複数あっても特定できますが、テストしていません。  
vJoyデバイスは15個まで選択できますが、実用的なのはたぶん1だけです。それ以外はテストしていません。
- ![Mapping](doc/ssMapping.png)![Mapping](doc/ssMapping2.png)
- マッピングでDS4のボタンをvJoyのボタンに割り当てる設定をします。
例えばL2やR2が押された場合ボタン15を有効にします。  
強制的 - L2Triggerやスティックなど、触れていない状態にも意味のある時に使います。  
トグル - on/offスイッチのように押し続けている状態が作れます。
- ![Repeat](doc/ssRapidFire.png)![Repeat](doc/ssRapidFire2.png)
- 割り当てたvJoyのボタンで連打を設定できます。  
初回時間経過後、解放と押下を繰り返します。  
同時押し条件を指定すると、たとえばL2やR2が押されている場合のみ、十字キーと□△×○の連打を有効にすることもできます。
- ![Keymap](doc/ssKeymap.png)![Keymap](doc/ssKeymap2.png)
- ボタンが押されたらキーダウンを、離されたらキーアップを送信できます。

## 予定
- 振動機能の強化
- バッテリー低下の時の警告

## Thanks
- DS4Windows https://github.com/Jays2Kings/DS4Windows
- http://eleccelerator.com/wiki/index.php?title=DualShock_4
- http://mclab.uunyan.com/lab/ds4/ds4_005.htm

## 開発環境
- Win7 64bit + Visual Studio 2015
- DUALSHOCK®4 CUH-ZCT1J01
- IO-DATA USB-BT40LE