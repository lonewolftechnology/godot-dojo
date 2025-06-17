# godot-dojo GDextension

Dojo no es soportado nativamente en Windows, por lo que en su caso hay que usar WSL

## Requerimentos:
- rustup
- cargo
- protobuf
- Instalar [Dojo](https://book.dojoengine.org/getting-started)
- scons / cmake (depende del que se quiera usar)
- Otros (avisen cualquier cosa)

## Para buildear con scons:
- Clonar repos de godot-cpp y dojo.c
```bash
git clone https://github.com/godotengine/godot-cpp -b godot-4.2.2-stable
git clone https://github.com/dojoengine/dojo.c -b v1.5.7

scons
```
Asumo que se puede buildear para otras plataformas:
Ejemplo: ``scons --platform=windows``

# Guida DOJO (KATANA, TORII):
Extracto de [starter](https://book.dojoengine.org/tutorial/dojo-starter)

Buildear el starter
```bash
sozo init dojo-starter
cd dojo-starter
sozo build
``` 
Levantar instancia KATANA
```bash
katana --dev --dev.no-fee --http.cors_origins "*"
```
En otra terminal ingresar ``sozo inspect``
Este comando muestra la info del server (contratos, etc, etc)
De aca lo mas importante es el address de dojo_starter-actions

Ahora hay que deployear a KATANA, asi obtenemos el adress del world
```bash
sozo migrate
```

Luego levantamos Torii, si el hex es diferente actualizenlo
```bash
torii --world 0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5 --http.cors_origins "*"
```

Ambas terminales deben permanecer abiertas.

## Projecto Godot

El projecto de [ejemplo](demo) deberia poderse abrir con cualquier version de godot >= 4.2.
Ignoren si les dice que se creo con 4.4 si usan una version diferente.

Consta de dos textfield que se pusieron por el solo proposito de testear mas facil.
El primero es para testear la conexion con el servidor. Recibe el address de world
El segundo es el que hace el login, te abre una pagina en el navegador. Recibe el address de actions

Si bien ambos ya tienen address constantes, no estoy seguro si estos cambian bajo el contexto dev.
