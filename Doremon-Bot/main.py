import discord
import os
import requests
import json
from better_profanity import profanity
import random
from replit import db
from keep_alive import keep_alive

client = discord.Client()

starter_counter_profanity = [
  "No u",
  "Cringe",
  "Ur mom gay",
  "KEKW OMEGALUL"
]

db["note_category_list"] = []
#######################QUOTES GENERATOR############################
def getQuote():
  response = requests.get("https://zenquotes.io/api/random")
  json_data = json.loads(response.text)
  quote = json_data[0]['q'] + " - Your Mother"
  return quote
############################################################

########################COUNTER PROFANITY########################
def update_counter_profanity(cp_msg):
  if "counter_profanity" in db.keys():
    counter_profanity = db["counter_profanity"]
    counter_profanity.append(cp_msg)
    db["counter_profanity"] = counter_profanity
  else:
    db["counter_profanity"] = [cp_msg]

def del_counter_profanity(index):
  counter_profanity = db["counter_profanity"]
  if len(counter_profanity) > index:
    del counter_profanity[index]
    db['counter_profanity'] = counter_profanity
################################################################

#########################NOTE TAKER#########################
def add_category(name):
  db[name] = []

def del_category(name):
  del db[name]

def add_note(categoryName, note):
  cm = db[categoryName]
  cm.append(note)
  db[categoryName] = cm

def del_note(categoryName, index):
  cm = db[categoryName]
  if len(cm) > index:
    del cm[index]
    db[categoryName] = cm

################################################################

@client.event
async def on_ready():
  print('We have logged in as {0.user}'.format(client))

@client.event
async def on_message(message):
  if message.author == client.user:
    return
  msg = message.content
  if msg.startswith("!command"):
    await message.channel.send("!hello, !encourage, !show_cp, !add_cp, !del_cp, !show_category, !show_note, !add_category, !del_category, !add_note, !del_note.")

  if msg.startswith('!hello'):
    await message.channel.send('Hello!')

  if msg.startswith("!encourage"):
    quote = getQuote()
    await message.channel.send(quote)
  
  options = starter_counter_profanity
  if "counter_profanity" in db.keys():
    options = options + db["counter_profanity"]

  if msg.startswith("!add_cp"):
    cp_msg = msg.split("!add_cp ", 1)[1]
    update_counter_profanity(cp_msg)
    await message.channel.send("Counter Profanity Updated!")

  if msg.startswith("!show_cp"):
    cp = []
    if "counter_profanity" in db.keys():
      cp = db["counter_profanity"]
      await message.channel.send(cp)

  if msg.startswith("!del_cp"):
    cp = []
    if "counter_profanity" in db.keys():
      index = int(msg.split("!del_cp ", 1)[1])
      del_counter_profanity(index)
      cp = db["counter_profanity"]
      await message.channel.send("Updated CP list is: " + str(cp))

  if profanity.contains_profanity(msg):
    await message.channel.send(random.choice(options))


  if msg.startswith("!add_category"):
    getName = msg.split()[1]
    if getName in db.keys():
      await message.channel.send("Category has already been used.")
    else:
      await message.channel.send("Creating new category....")
      add_category(getName)
      temp = db["note_category_list"]
      temp.append(getName)
      db["note_category_list"] = temp
      await message.channel.send("New category created!")
  
  if msg.startswith("!del_category"):
    getName = msg.split()[1]
    if getName in db.keys():
      await message.channel.send("Deleteing category " + getName)
      del_category(getName)
      lst = db["note_category_list"]
      newlst = db["note_category_list"]
      count = 0
      for i in lst:
        if i == getName:
          newlst.pop(count)
          break
        count= count + 1
      db["note_category_list"] = newlst
    else:
      await message.channel.send("There is no such category exist.")
    
  if msg.startswith("!add_note"):
    if msg.count(" ") == 2:
      getCategory = msg.split()[1]
      getNote = msg.split()[2]
      for i in db.keys():
        print(i)
      if getCategory in db.keys():
        add_note(getCategory, getNote)
        await message.channel.send("Successfully added note!")
      else:
        await message.channel.send("Can't add note. Please try again.")
    else:
      await message.channel.send("Need to include category and you notes with a space in between.")

  if msg.startswith("!del_note"):
    getCategory = msg.split()[1]
    getIndex = msg.split()[2]
    if getCategory in db.keys():
      del_note(getCategory, int(getIndex))
      await message.channel.send("Successfully deleted note!")
    else:
      await message.channel.send("Can't delete note. Please try again.")
    
  if msg.startswith("!show_category"):
    category = []
    if "note_category_list" in db.keys() and len(db["note_category_list"]) > 0:
      category = db["note_category_list"]
      await message.channel.send(category)
    elif len(db["note_category_list"]) == 0:
      await message.channel.send("Category is Empty now.")
    else:
      await message.channel.send("No such thing exist.")

  if msg.startswith("!show_note"):
    getName = msg.split("!show_note", 1)[1]
    if getName in db.keys():
      name = []
      name = db[getName]
      await message.channel.send(name)
    else:
      await message.channel.send("No such category exist. Please try again.")




keep_alive()

client.run(os.getenv('TOKEN'))