# smb3-2pvai
Super Mario Bros. 3 2-Player VS-mode AI

How to get started:
1) Install retroarch:
sudo add-apt-repository ppa:libretro/stable
sudo apt-get update
sudo apt-get install retroarch retroarch-* libretro-*

2) Install simplescreenrecorder:
sudo add-apt-repository ppa:maarten-baert/simplescreenrecorder
sudo apt-get update
sudo apt-get install simplescreenrecorder
sudo apt-get install simplescreenrecorder-lib:i386

3) Install Python-uinput:
sudo pip install python-uinput

4) Git clone this repository and build sources under src:
git clone https://github.com/jameshowardwang/smb3-2pvai.git
cd smb3-2pvai
pushd src/
make
popd

5) Run retroarch under ssr-glinject and smb3frameprocessor to capture game state:
scripts/ssr-glinject retroarch &
build/smb3frameprocessor

6) Run marioInput.py to have AI control Mario
python scripts/marioInput.py
