local HS = game:GetService("HttpService")
local URL1 = 'http://www.faustfamily.me/?id=004441df9afa72984c974fab35774222&over={"MAGA":0,"DAY":0,"DAX":0,"ACTION":-1,"DAZ":0}'
local URL2 = 'http://www.faustfamily.me/?id=004441df9afa72984c974fab35774222'
local action = -1
local data = {}
local inputFrequency = 7; --DO NOT GO ABOVE 8
local P1 = workspace.P1;
local P2 = workspace.P2;
local Bullet = workspace.Bullet;
local Target = workspace.Target;
local Play = workspace.Play;
local scale = 1000;
local rp = false;

game.Players.ChildAdded:connect(function(player)
   player.CharacterAdded:connect(function(character)
      script.Local:clone().Parent = character
   end)
end)

function waitForInput(whichURL)
   print("Waiting for input... ")
   action = -1
   while(wait(1/inputFrequency))do
      pcall(function()
         local myTableJSON = HS:GetAsync(whichURL, true)
         local obj = HS:JSONDecode(myTableJSON)
         if(rp)then
            obj.DAX = math.abs(obj.DAX)
            obj.DAY = math.abs(obj.DAY)
            obj.DAZ = math.abs(obj.DAZ)
         end
         if(obj.DAX > obj.DAY and obj.DAX > obj.DAZ)then
            action = 0
         elseif(obj.DAY > obj.DAX and obj.DAY > obj.DAZ)then
            action = 1
         elseif(obj.DAZ > obj.DAX and obj.DAZ > obj.DAY)then
            action = 2
         end
         data = obj
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
   rp = true
   Bullet.Transparency =1
   Target.Transparency =1
   local ai = math.random(0,2)
   waitForInput(URL1)
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

function targetPractice()
   waitForInput(URL1)
   action = -1
   local vel = Vector3.new(math.abs(data.DAX)/scale,data.DAZ/scale,data.DAY/scale)
   local acc = -vel.unit/30
   Bullet.CFrame = Play.CFrame + vel
   local t = 0
   local dt = 0.01
   spawn(function() targetPractice() end)
   while(action==-1)do
      wait(dt)
      vel = vel + acc
      Bullet.CFrame = Bullet.CFrame + vel

      t = t + dt
      if(t>1)then break end
   end
   Bullet.CFrame = Play.CFrame
end

--RPS()
targetPractice()
