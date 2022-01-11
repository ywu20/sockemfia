# sockemfia

## By Sarah Leong, Yuqing Wu, Andy Lin pd 5
## Description of Project

This will be an online multiplayer game where people play Mafia, an epic party game.  

Try to vote out the mafia! (or kill the good people and hide yourself if you are a mafia)  

Roles:

Bad people:  
- Mafia: Can kill one person at night.  
- Lead mafia: Cast the ultimate decision regarding who to kill on the computer.  

Good people:  
- Civilians: regular people. Try to protect special roles!  
special roles:  
- Detective: can check if a person is good or bad. Check one person per night.  
- Doctor: can kill a person with poison and save a person with medicine during the entire game, but can't kill and save at the same night.  
- Hunter: can kill a person when the hunter dies.  

## A description as to how the project will be used (describe the user interface).
1. The user will first enter a name that they are going to be called during the game.  

2. The user then gets a role assigned to them. Possible roles are civilian, mafia, lead mafia, doctor, detective, and perhaps a hunter.  

3. The night comes, users will be directed to do things based off of their roles. (Advanced feature: we might have everybody to type something all the time so that the mafia won't be discovered when people are playing in the same room because they typed, but this may be unecessary)  

4. Announce who died in the night.  

5. People go around and chat to explain why they are not mafia and who they vote for and why.  

6. People vote for mafia.  

7. Announce who got voted out.  

8. Repeat steps 3-7 until all mafia die (good people win) or all special roles die or all civilian die (bad people win). Or maybe you guys want to do mafia win if they outnumber good people. 

## Technical Design. 
   #### How you will be using the topics covered in class in the project.
   - We will be using a sockets in order to create a server to connect multiple clients to a server.
   - Semaphores will be used to cap the number of people who are in the game.
   -  Working with files will be used to have a database of user information (roles, votes, whether they died or not etc). Write binary files so people can't cheat.
   -  Using shared memory to keep track of things that multiple programs modify. 
   -  Using forking in order to have a timer for voting and discussion phases of the game.
   #### How you are breaking down the project and who is responsible for which parts.
   - Explain game rules at the start. - Yuqing
   - Semaphore - Andy
      - When the server starts, the user that hosts the server will be asked to enter how many people are in the game, and that will be the number of semaphores we give.
   - Role assignment, each client is assigned a role when they first connect to the server, record their roles in a file. - Yuqing
   - Instructional period where people are killed and special roles are doing special things. - Yuqing (maybe a separate mafia chatroom (Sarah)) 
      - use database file to record who got killed / saved. 
   - Announce who died. - Yuqing
   - Chatroom where every person goes around and talk. The server will receive what each client enters and podcast it to all the other clients. - Sarah
   - Voting: each person votes for who they think the mafia is. The vote is announced to the clients so that people can see what other people voted for. Record who's dead. - Andy
   - Timer to ensure that people don't try to halt the game by refusing to vote or to ensure that people only discuss within a set time. - Andy
   - The people who are dead can continue watch the game but can't write anything to the server or can write to a separate chat. - Sarah
   - End game, announce results. - Andy
   #### What algorithms and /or data structures you will be using, and how.
   - A struct will keep track of the state of a player (alive, role, ...), as well as the IP address and port the client is on.
   - Each player will get a number (ID) and the file that stores their information would be named like that. 
   - An array of structs will be used in order to have the statuses of all players in the game.

## Timeline 
1/13: Role assignment, semaphore, instructional period for mafia to kill people, announce death.  
1/18: Chatroom for both mafia and big group, voting.  
1/20: Deal with dead players, special roles instructional period.  
1/24: Debugging, error handling, timer.  
