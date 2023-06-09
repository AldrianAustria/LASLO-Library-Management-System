
from chatterbot import ChatBot
from chatterbot.trainers import ChatterBotCorpusTrainer

chatbot = ChatBot("Laslo")
trainer = ChatterBotCorpusTrainer(chatbot)

trainer.train("chatterbot.corpus.english.computers")

name=input("Enter Your Name: ")
print("Welcome to the Bot Service! Let me know how can I help you?")
while True:
    request=input(name+': ')
    if request=='Bye' or request =='bye':
        print('Bot: Bye')
        break
    else:
        response=chatbot.get_response(request)
        print('Bot:',response)

