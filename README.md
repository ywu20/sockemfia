# sockemfia

## By Sarah Leong, Yuqing Wu, Andy Lin pd 5
## Description of Project

This will be an online multiplayer game where people play Mafia, an epic party game.

## A description as to how the project will be used (describe the user interface).
## Technical Design. 
   #### How you will be using the topics covered in class in the project.
   - We will be using a sockets and forking in order to create a forking server to connect multiple clients to a server.
   - Semaphores will be used to cap the number of people who are in the game.
   -  Working with files will be used to have a database of user information (roles, votes, whether they died or not etc). Write binary files so people can't cheat.
   #### How you are breaking down the project and who is responsible for which parts.
   - Semaphore: Andy
      - When the server starts, the user that hosts the server will be asked to enter how many people are in the game, and that will be the number of semaphores we         give. 
   - Role assignment, each client is assigned a role when they first connect to the server, record their roles in a file. 
   - Instructional period where people are killed and special roles are doing special things. (maybe a separate mafia chatroom)
      - use database file to record who got killed / saved. 
   - Announce who died.
   - Chatroom where every person goes around and talk. The server will receive what each client enters and podcast it to all the other clients. 
   - Voting: each person votes for who they think the mafia is. The vote is announced to the clients so that people can see what other people voted for. Record          who's dead.
   - The people who are dead can continue watch the game but can't write anything to the server. 
   #### What algorithms and /or data structures you will be using, and how.
   - A struct will keep track of who's alive, dead, doctor, mafia, detective.
   - An array of structs will be used in order to have the statuses of all players in the game.

## Timeline 

