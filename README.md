# pg_dummy

PostgreSQL のダミー拡張モジュール pg_dummy のビルドと、
deb パッケージの作成と、
apt repository の構築までを、
なんとなくメモしていきます。

業界の最適な策を実践をしていないかもしれませんが、その点はご容赦ください。

## Docker で作業

Docker で試せるようにしました。

`docker` フォルダーにて、

```bat
docker-compose up -d

docker exec -it builder bash
```

docker-compose.yml に書いてありますが、次のフォルダーをマウントしています。

- apt
- src

### pg_dummy をビルド

bash に入ったら、順次実行してください。

```sh
mkdir /build
cd /build
cmake ../src
make

make package
```

`make package` の出力がこういう感じになっていれば、パッケージは作成できています。

```txt
root@5998cba9fd40:/build# make package
[100%] Built target pg_dummy
Run CPack packaging tool...
CPack: Create package using DEB
CPack: Install projects
CPack: - Run preinstall target for: Project
CPack: - Install project: Project
CPack: Create package
CPack: - package: /build/postgresql-10-pg_dummy_0.1.0-1_amd64.deb generated.
root@5998cba9fd40:/build#
```

### gpg キーを作成

deb パッケージと、Release ファイルに署名するための GPG キーペアを作成してください。

参考: [How To Create an Authenticated Repository](https://help.ubuntu.com/community/CreateAuthenticatedRepository)

実行

```sh
gpg --gen-key
```

本名とメールアドレスとパスワードを尋ねられます。

メッセージの途中に表れるこちら `97EF4BE15A920E4E` が捺印でしょうか。

`gpg: key 97EF4BE15A920E4E marked as ultimately trusted`

### gpg キーをバックアップ

Docker で作業していると、コンテナを破棄した時に GPG キーペアが無くなってしまいます。

こちらの手順を参考に、バックアップしておいてください。

参考: [GPGの秘密鍵や公開鍵のバックアップ方法](https://www.rk-k.com/archives/3351)

### gpg キーを pgp.mit.edu へアップ

そういうことができるらしいので、アップしてみました。

参考: [gpg キーを更新した](https://qiita.com/takayama/items/cb9a47d3a1f9fae8a205)

コマンド

```sh
gpg --keyserver pgp.mit.edu --send-keys 97EF4BE15A920E4E
```

出力

```出力
root@5998cba9fd40:/build# gpg --keyserver pgp.mit.edu --send-keys 97EF4BE15A920E4E
gpg: sending key 97EF4BE15A920E4E to hkp://pgp.mit.edu
```

### deb ファイルを署名

コマンド

```sh
export GPG_TTY=$(tty)

dpkg-sig --sign builder *.deb
```

出力

```txt
root@5998cba9fd40:/build# dpkg-sig --sign builder *.deb
Processing postgresql-10-pg_dummy_0.1.0-1_amd64.deb...
Signed deb postgresql-10-pg_dummy_0.1.0-1_amd64.deb
```

### apt/conf/distributions

`apt/conf/distributions` をエディターで編集しておいてください。

```sh
Origin: me
Label: myrepo
Codename: myrepo
Architectures: amd64
Components: main
Description: myrepo
SignWith: 97EF4BE15A920E4E
```

特に `SignWith: 97EF4BE15A920E4E` は `gpg --list-keys` などで表示されるハッシュ値に変更しておかないと、
作業が失敗します。

### レポジトリを生成

どうやら、これで空のレポジトリを生成できるみたいです。

参考: [How to Create a Private Debian Repository with Reprepro](https://www.codepool.biz/linux-debian-reporisory-reprepro.html)

```sh
cd /apt
reprepro export
```

### deb パッケージを追加

コマンド

```sh
reprepro includedeb myrepo /build/*.deb
```

出力

```txt
root@23b129ea7f5b:/apt# reprepro includedeb myrepo /build/*.deb
Exporting indices...
```

作成はこれで完了…

`/apt` フォルダをどこかの web サーバーへホスティングしてください。
その際に `db` と `conf` は取り除くか、またはアクセスできないようにしておいてください。

### apt install する時

```sh
echo deb http://SERVER/apt myrepo main > /etc/apt/sources.list.d/myrepo.list

apt-key adv --keyserver pgp.mit.edu --recv-keys 97EF4BE15A920E4E

apt update

apt install postgresql-10-pgdummy
```
