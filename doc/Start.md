## Ft_irc

#### Objectif
- Le but de **ft_irc** est de coder un serveur IRC (Internet Relay Chat) en C++ avec le même fonctionnement qu'un vrai serveur IRC.
	* On utilisera un vrai client IRC pour se connecter à notre serveur et le tester.

#### Background
- **IRC (Internet Relay Chat)** est un protocole de communication textuel instantané sur Internet.
- La communication se fait souvent en groupe via des canaux.
- Elle peut aussi être directe entre deux clients.
- Les clients IRC se connectent à des serveurs IRC pour accéder aux canaux de communication.
- Les serveurs IRC peuvent aussi se connecter entre eux pour créer des réseaux.

#### Consignes à respecter dans le projet :

- Le programme ne doit pas crasher.
- Il faut être attentif aux cas où la mémoire est insuffisante.
- Le programme ne doit pas s'arrêter de manière inattendue, sauf dans des cas indéfinis.
---------
- Mon Makefile ne doit pas relinker et doit contenir les règles suivantes : `$(NAME)`, `all`, `clean`, `fclean`, et `re`.
- Mon code doit compiler avec **C++** en utilisant les flags `-Wall -Werror -Wextra`.
- Mon programme doit être codé conformément à la norme **-std=c++98**.
- Mon code doit être écrit uniquement en **C++**. Les bibliothèques externes, y compris Boost, sont interdites.
---------

### Programme :

* Nom exécutable : **ircserv**
* Rendu : **Makefile**, fichiers `*.{h, hpp, cpp, tpp, ipp}`, et un fichier de configuration optionnel.
* Arguments : `port` (le port d'écoute) et `password` (le mot de passe de connexion).
* Fonctions autorisées : `socket`, `close`, `setsockopt`, `getsockname`, `getprotobyname`, `gethostbyname`, `getaddrinfo`, `freeaddrinfo`, `bind`, `connect`, `listen`, `accept`, `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`, `send`, `recv`, `signal`, `sigaction`, `lseek`, `fstat`, `fcntl`, `poll` (ou équivalent), ainsi que toutes les fonctions conformes à la norme **C++98**.

###### Remarque : Bien que `poll()` soit mentionné dans le sujet et la grille d'évaluation, vous pouvez utiliser un équivalent tel que `select()`, `kqueue()`, ou `epoll()`.

### Instructions :

1. **Je dois développer un serveur IRC en C++98.**
2. **Je ne dois pas** développer un client.
3. **Je ne dois pas** gérer la communication entre serveurs.
4. Mon binaire doit être appelé de la manière suivante :

```sh
./ircserv <port> <password>
```

* `port` : Le numéro du port sur lequel le serveur acceptera les connexions entrantes.
* `password` : Le mot de passe permettant de s'identifier auprès du serveur IRC, à fournir par tout client souhaitant s'y connecter.

### Exigences :

1. Le serveur doit gérer plusieurs clients sans jamais bloquer.
2. Le **forking** est interdit, et toutes les opérations I/O doivent être non bloquantes.
3. J'ai le droit à un seul `poll()` pour gérer toutes les opérations (lecture, écriture, écoute, etc.).

###### Remarque :
Bien que vous puissiez utiliser des descripteurs de fichiers (FD) en mode non bloquant avec `read/recv` ou `write/send` sans utiliser `poll()` (ou équivalent), cela consommerait inutilement des ressources système. Si vous tentez cette méthode, votre note sera de **0**.

4. Je dois choisir un client comme **référence** pour l'évaluation.
5. Le client **référence** doit se connecter sans erreur.
6. La connexion entre le serveur et le client se fera en **TCP/IP**.

7. Le client IRC doit avoir les fonctionnalités suivantes :

	- Authentification, définition d'un **nickname**, d'un **username**, rejoindre un **channel**, **envoyer** et **recevoir** des messages privés.
	- Les messages envoyés dans un **channel** doivent être retransmis à tous les clients ayant rejoint le channel.
	- Gestion des **opérateurs** et des **utilisateurs basiques**.

8. Je devrai implémenter les commandes suivantes pour les opérateurs :
	- **KICK** : Éjecter un client du channel.
	- **INVITE** : Inviter un client à un channel.
	- **TOPIC** : Modifier ou afficher le sujet du channel.
	- **MODE** : Changer le mode du channel :
		* **i** : Définir/supprimer le mode invitation uniquement.
		* **t** : Restreindre ou non la commande TOPIC aux opérateurs.
		* **k** : Définir/supprimer une clé (mot de passe) pour le channel.
		* **o** : Accorder/retirer le privilège d'opérateur.
		* **l** : Définir/supprimer une limite d'utilisateurs pour le channel.

###### Remarque :
Vous pouvez utiliser `fcntl()` uniquement comme suit :
```c++
	fcntl(fd, F_SETFL, O_NONBLOCK);
```
Tout autre flag est interdit.

### Tests et évaluations :

* Je dois vérifier tous les types d'erreurs possibles, y compris les données partiellement reçues et les connexions à faible bande passante.

Exemple de test :
```sh
$> nc 127.0.0.1 6667
com^Dman^Dd
$>
```
Utilisez `Ctrl+D` pour envoyer la commande en plusieurs parties : `com`, puis `man`, puis `d\n`. Pour traiter une commande, il faudra d'abord la reconstituer en concaténant les paquets reçus.

### Partie Bonus :
- Envoi de fichiers.
- Création d'un bot.
