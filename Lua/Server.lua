local HS = game:GetService("HttpService")
local URL = 'http://panopticon.ballistaline.com/pebble.php?id=004441df9afa72984c974fab35774222&over={"ACTION":-1}'
local action = -1
local inputFrequency = 7; --DO NOT GO ABOVE 8
local P1 = workspace.P1;
local P2 = workspace.P2;

game.Players.ChildAdded:connect(function(player)
   player.CharacterAdded:connect(function(character)
      script.Local:clone().Parent = character
   end)
end)

function waitForInput()
   print("Waiting for input... ")
   action = -1
   while(wait(1/inputFrequency))do
      pcall(function()
         local myTableJSON = HS:GetAsync(URL, true)
         local obj = HS:JSONDecode(myTableJSON)
         action = obj.ACTION
      end)
      if(action~=-1)then break end
   end
   print("Recieved: "..action)
end

function choiceString(x)
   if(x==0)then
      return "Rock"
   elseif(x==1)then
      return "Paper"
   elseif(x==2)then
      return "Scissors"
   else
      return "Invalid"
   end
end

function spawnParts(ai)
   local parts = {}
   part1 = game.ServerStorage[choiceString(ai)]:Clone()
   part1.Parent = workspace
   part1.CFrame = CFrame.new(P1.Position)
   part2 = game.ServerStorage[choiceString(action)]:Clone()
   part2.Parent = workspace
   part2.CFrame = CFrame.new(P2.Position)
   table.insert(parts,part1)
   table.insert(parts,part2)
   return parts
end

function explode(part)
   spawn(function()
      local e = Instance.new("Explosion")
      e.Parent = workspace
      e.Position = part.Position
      wait(2)
      e:Destroy()
   end)
end

function Destroy(parts)
   for k,v in pairs(parts) do
      v:Destroy()
   end
end

function RPS()
   local ai = math.random(0,2)
   waitForInput()
   print(choiceString(action).." vs "..choiceString(ai))
   local parts = spawnParts(ai)
   wait(1)
   if(ai==action)then
      print("Tie.")
   elseif(ai==action+1 or (ai==0 and action==2))then
      print("You lose.")
      explode(P2)
   elseif(action==ai+1 or (action==0 and ai==2))then
      print("You win!")
      explode(P1)
   else
      print("Undefined behavior.")
   end
   wait(1)
   Destroy(parts)
   RPS()
end

RPS()
