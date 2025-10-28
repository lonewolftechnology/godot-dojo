## Overview

Este proyecto contiene lo básico para utilizar la GDExtension de DojoC en Godot.
El contrato usado es una version modificada de Dojo-Starter
Este proyecto no esta configurado completamente.

Este documento es una guia para el usuario para completar la configuración final del proyecto para poder ejecutarlo

## Configuration

Para la ejecucion de este juego se deben configurar los parametros del `ToriiClient` y `ControllerAccount`

Existen tres maneras posibles de configurar los parametros para estos nodos, con distinto orden de precedencia

1. Usando los parametros de los respectivos métodos de cada uno
2. Configurando los parametros desde el editor
3. Configurando los project settings

En este documento se utilizara la tercer opcion. Que es la de menor orden de presedencia.

1. Abra las project settings desde su menu
2. Navegue hasta la sección de Dojo (que se deberia encontrar al final de las opciones)
3. Configure:
	- **Katana URL**: Con el url de su servidor de Katana. 
Puede utilizar: `https://api.cartridge.gg/x/godot-demo-rookie/katana`
	- **Torii URL**: Con el url de su servidor de Torii. 
Puede utilizar: `https://api.cartridge.gg/x/godot-demo-rookie/torii`
	- **Torii World Address**: Con los addresses de lo mundos de Torii.
Este demo solo utiliza el siguiente address: `0x03a7e28319f3617da4135893c711c79a1306adcd87cca4fbd0ceda50ae397683`
	- **Contract Address**: Con el address de su contrato.
Puede utilizar: `0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da`
	- **Policies**: Con las policies que utiliza su contrato.
Las policies usads por este contrato son:
 		- "move": "Move in the world"
		- "spawn": "Spawn in the world"

	
