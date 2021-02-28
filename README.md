<p><H3>ESP32�ɂ��C���^�[�l�b�g���W�I</H3></p>
<p>
MP3�X�g���[����I2S�f�[�^�փf�R�[�h�ł���<a href="https://github.com/schreibfaul1/ESP32-audioI2S">���C�u����</a>�𗘗p�����ȒP�\���̃C���^�[�l�b�g���W�I�ł��B<br>
�ŏ��\����ESP32��I2S DAC������ΐ���ł��܂��B<br>
Web�T�[�o�[�𓋍ڂ��Ă���̂ŁA�u���E�U���瑀��ł��܂��i�^�N�g�X�C�b�`���s�v�j�B<br>
Arduino IDE��<a href="https://github.com/espressif/arduino-esp32">ESP32�J����</a>���K�v�ł��B
</p>

<p><strong>����</strong><br>
 �E�\�����ȒP�ň����B<br>
 �EWiFi�ڑ��B<br>
 �EMP3�X�g���[���̃C���^�[�l�b�g���W�I�ǂ���M�ł���B<br>
 �E�u���E�U���瑀��ł���B<br>
 �E�u���E�U�ɃA�[�e�B�X�g���A�Ȗ����\���i���{��j�����i����ꍇ�͕\�����u�i���{��s�j�ɂ��j�B<br>
 �E�C���^�[�l�b�g���W�I�ǂ̐ݒ�̓v���Z�b�g�܂���URL����͐ݒ�B<br>
 �E�X���[�v�^�C�}�[�i�U�O���j�t���B<br>
</p>
<p><strong>H/W�\��</strong><br>
 �EESP32 - ESP WROOM 32 DevKitC (30PIN�̂��̂𐄏�)<br>
 �EUDA1334 I2S DAC �܂��� PCM5102 I2S DAC<br>
 �ESD1306 128x64 OLED�\�����u�i�I�v�V�����j<br>
</p>
<p>
<img src="iradio.jpg" width="480" height="360">
<p>����LED��POWER�A��LED��SLEEP ON�A���ʒ����p�̃^�N�g�X�C�b�`�i2�j�͖��ڑ�</p>
</p>
<p><strong>����</strong><br>
 �E�v���Z�b�g�����C���^�[�l�b�g���W�I�ǂ̐؂�ւ��i�u���E�U�^�^�N�g�X�C�b�`�j�B<br>
 �E���ʒ����i�u���E�U�^�^�N�g�X�C�b�`�j�B<br>
 �E�X���[�v�^�C�}�[��ON/OFF�i�u���E�U�^�^�N�g�X�C�b�`�j�B<br>
 �E�C���^�[�l�b�g���W�I�ǂ�URL���́A�ݒ�ƕۑ��i�u���E�U�j�B<br>
</p>
<p><strong>�ڑ�</strong><br>
<p>�e�R���|�[�l���g�̐ڑ��͈ȉ��̒ʂ�B<br>
���ڑ�PIN�̓��C�u�����̃I���W�i���̂��̂Ƃ͕ύX���Ă���̂Œ���<br>
</p>
<p>
I2S DAC
<table> 
<tr>
<td>I2S DAC(UDA1334)&nbsp;</td><td>I2S DAC(PCM5102)&nbsp;</td><td>ESP32</td>
</tr>
<tr>
<td>VIN</td><td>VIN</td><td>3V3</td>
</tr>
<tr>
<td>GND</td><td>GND</td><td>GND</td>
</tr>
<tr>
<td>WSEL</td><td>LCK</td><td>D22</td>
</td>
<tr>
<td>DIN</td><td>DIN</td><td>D25</td>
</tr>
<tr>
<td>BCLK</td><td>BCK</td><td>D26</td>
</tr>
<tr>
<td>-</td><td>SCK</td><td>GND</td>
</tr>
</table>
</p>
<p>
I2C �\�����u
<table> 
<tr>
<td>I2C&nbsp;</td><td>ESP32</td>
</tr>
<tr>
<td>VCC</td><td>3V3</td>
</tr>
<tr>
<td>GND</td><td>GND</td>
</tr>
<tr>
<td>SCK</td><td>D4</td>
<tr>
<tr>
<td>SDA</td><td>D17(TX2)</td>
<tr>
</table>
</P>
<p>
�^�N�g�X�C�b�`�i�{�^���j
���u���E�U���瑀��ł���̂Ŗ����Ă�OK�B
<table> 
<tr>
<td>�@�\&nbsp;</td><td>ESP32&nbsp;</td><td>�@�\</td>
</tr>
<tr>
<td>SLEEP</td><td>D32</td></td><td>SLEEP�@�\��ON/OFF</td>
</tr>
<tr>
<td>VOL-</td><td>D14</td><td>���ʏ��i�ŏ�0�j</td>
</tr>
<tr>
<td>VOL+</td><td>D12</td><td>���ʑ�i�ő�20�j</td>
<tr>
<tr>
<td>STATION</td><td>D13</td><td>�C���^�[�l�b�g���W�I�ǂ��u���ցv�؂�ւ�</td>
<tr>
</table>
</p>
</p>
<p><strong>�C���X�g�[��</strong><br>
<ol>
<li>�R�[�h���AZIP�`���Ń_�E�����[�h</li>
<li>ArduinoIDE�ɂ����ă��C�u�����Ƃ��ē����i�I���W�i����MP3-I2S�̃��C�u��������荞��ł���j</li>
<li>ArduinoIDE����ESP32_iradio_srv.ino���J��</li>
<li>�`����ssid = "xxxxxxxx"; �� password = "pppppppp"; ��2�s��<br>���p���Ă���WiFi�|�C���g�̂��̂ɏ���������</li>
<li>�u���؁E�R���p�C���v�ɐ���������A��U�A�u���O��t���ĕۑ��v���s��<br>
�i���C�u�������s���̃G���[���o����A�ǉ�����B�Ⴆ�΁ASD1306 OLED�\�����u�j
</li>
<li>��ɏq�ׂ�H/W�\���i�ŏ��\���Ƃ��āAESP32��DAC���K�v�j��p�ӂ��A�z����ڑ�����</li>
<li>PC��USB�P�[�u���Őڑ�����B�K����COM�ԍ��ŔF�������iUSB�̃h���C�o���K�v�ȏꍇ����j</li>
<li>�u�}�C�R���{�[�h�ɏ������ށv���s��</li>
<li>�������݂���������΁A����̏󋵂��V���A�����j�^�ɕ\�������</li>
<li>WiFi�ڑ����AIP�A�h���X���V���A�����j�^�ɕ\�������B�u���E�U����"http://IP�A�h���X"��<br>�A�N�Z�X����ƈȉ��̉�ʂ��\�������<br>
</li>
</ol>
</p>
<p>
<img src="browser1.jpg" width="400" height="400">
<p>�C���^�[�l�b�g���W�I�ǂ�URL�A�A�[�e�B�X�g���A�Ȗ��AVOLUME�̒l�A�v���Z�b�g�ς�URL���\������Ă���<br>
�uOperation�v���N���b�N����ƈȉ��̑����ʂɑJ�ڂ���</p>
</p>
<p>
<img src="browser2.jpg" width="800" height="256">
<p>�e����̃{�^���B�uStation URL�v�Ɉꎞ�I�ȃC���^�[�l�b�g���W�I�ǂ�URL���w��i�uSET�v�{�^���j�ł��A<br>��MOK�̏ꍇ�A�uSAVE�v���N���b�N����ƕۑ�����āA
����̃p���[�I�����Ɏ����ڑ�����B<br>�uSTATION�v�{�^�����N���b�N����ƕۑ������������
</p>
</p>
<p><strong>���ӎ���</strong><br>
<p>�E����󋵁A�C���^�[�l�b�g���W�I�ǂ̃f�[�^�X�g���[���`���ɂ���Ă͉����r�؂�邱�Ƃ�����܂��B<br>
�E���p�̍ۂ́A���ȐӔC�ł��y���݂��������B</p>
</P>
