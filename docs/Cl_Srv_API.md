Client-server communication is handled via pickled arrays.
The first parameter in the array is the message type.
The second is determined by it.

### Server to client messages

- `id`

    Parameter: `player_id` (int)
    
    First message sent to a player, supplies their randomly generated ID.
    The player is expected to reply with an `acknowledge`, either accepting
    it or supplying their own.
  
- `reject`

  Parameter: `reason` (str)

  Possible response to an `acknowledge` request.
  If the player with the specified ID is already dead or there are too many players connected, 
  this message signifies the client cannot connect.
  
  May also happpen before `id` if all the players are already connected and active.
  
- `move`

    Parameter: `game_state` (dict)
    
    The game state is sent to the player whose turn it is to move. 
    The player is expected to reply with an `action`.
  
- `state`

    Parameter: `game_state` (dict)
    
    The updated game state is sent to all active players who aren't moving yet.
    They are not expected to reply.
  
- `shutdown`

    Parameter: 'message' (str)
    
    Signals the game was shut down or is over.

### Client to server messages

- `acknowledge`
    
    Parameter: `player_id` (int)
  
    A response to an `id` message.
    The player may set their own ID via the client.
    If he does so, that ID is sent to the server.
    If not, the player accepts the server-assigned ID.
  
- `action`
    
    Parameter: `player_command` (pair of int and action)

    A response to a `move` message.
    Sends the player input back to the server.

- `death`

  Parameter: `dummy` (bool)

  Command to signal the player is dead.
  
- `exit`

    Parameter: `dummy` (bool)
  
    Command to shut down the server.