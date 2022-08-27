import discord
from discord.ext import commands
from discord.ext.commands import Bot
import random
import requests
import io
from PIL import Image, ImageFont, ImageDraw
import datetime

pashal = [ 'кодзима' ]
hello_words = [ 'hello', 'hi', 'ку', 'привет', 'даров', 'дароу', 'хай', 'прив', 'приветики', 'здарва', 'ky', 'хело', 'хелоу']
goodbye_words = [ 'пока', 'всем пока', 'дозавтра', 'увидемся', 'я пошел', 'я пошёл' ]
bad_words = [ 'сука', 'бля', 'пизда', 'лох', 'чмо', 'пидорас', 'шлюха', 'блять', 'писька', 'жопа', 'очко', 'пидр']
monetka = random.randint(1,2)
russian_rulette = random.randint(1,6)

TOKEN = 'Njg3NzA3NTU3Nzc5MDc5MTg4.XohT-A.g4pcP_ZkGL7Uzq9dfZPs7k_bswE'

Bot = commands.Bot(command_prefix= '!')
Bot.remove_command('help')

@Bot.event
async def on_ready():
    print("Bot is online")
    await Bot.change_presence(status= discord.Status.online, activity= discord.Game('танки'))

@Bot.command()
@commands.has_permissions(administrator= True)
async def clear(ctx, amount= 100):
    await ctx.channel.purge(limit=amount)

@Bot.command()
@commands.has_permissions(administrator= True)
async def kick(ctx, member: discord.Member, *, reason = None):
    #emb = discord.Embed(title= 'KICK', colour= 0x770a5b)
    await ctx.channel.purge(limit= 1)
    await member.kick(reason = reason)
    #emb.set_author(name= member.name, icon_url= member.avatar_url)
    #emb.add_field(name= '', value= 'kick user:' f'{member.mention}')
    #emb.set_footer(text= f'Был забанен администратором сервера {ctx.author.name}', icon_url= ctx.author.avatar_url)
    #await ctx.send(embed= emb)
    await ctx.send(f'kick user{member.mention}')

@Bot.command()
@commands.has_permissions(administrator= True)
async def ban(ctx, member: discord.Member, *, reason = None):
    #emb = discord.Embed(title= 'BAN', colour= 0x770a5b)
    await ctx.channel.purge(limit= 1)
    await member.ban(reason=reason)
    #emb.set_author(name= member.name, icon_url= member.avatar_url)
    #emb.add_field(name= '', value= 'ban user:' f'{member.mention}')
    #emb.set_footer(text= f'Был забанен администратором сервера {ctx.author.name}', icon_url= ctx.author.avatar_url)
    #await ctx.send(embed= emb)
    await ctx.send(f'ban user {member.mention}')

@Bot.command()
@commands.has_permissions(administrator= True)
async def unban(ctx, *, member):
    await ctx.channel.purge(limit= 1)
    banned_users = await ctx.guild.bans()
    for ban_entry in banned_users:
        user = ban_entry.user
        await ctx.guild.unban(user)
        await ctx.send(f'unbanned user {user.mention}')
        return

@Bot.command()
async def help(ctx):
    await ctx.channel.purge(limit= 1)
    emb = discord.Embed(title= 'Навигация по камандам:', colour= 0x770a5b)
    emb.add_field(name= '{}монетка'.format('!'), value= 'игра для решения споров')
    emb.add_field(name= '{}русская рулетка'.format('!'), value= 'проверь свою удачу')
    await ctx.send(embed= emb)

@Bot.command()
@commands.has_permissions(administrator= True)
async def mute(ctx, member: discord.Member):
        await ctx.channel.purge(limit= 1)
        mute_role = discord.utils.get(ctx.message.guild.roles, name = 'говорун')
        await member.add_roles(mute_role)
        await ctx.send(f'mute user {member.mention}')

@Bot.event
async def on_member_join(member):
    channel = Bot.get_channel(687661562458275850)
    role = discord.utils.get(member.guild.roles, id= 695930826529046538)
    await member.add_roles(role)
    await channel.send(embed= discord.Embed(description= f'Теперь ты имеешь место в этом мире {member.name}'))

@Bot.event
async def on_message(message):
    await Bot.process_commands(message)
    msg = message.content.lower()
    if msg in hello_words:
        await message.channel.send('Привет, привет ' f'{message.author.mention}')
    if msg in goodbye_words:
        await message.channel.send('До встречи ' f'{message.author.mention}')
    if msg in bad_words:
        await message.delete()
        await message.channel.send(f'Пожалуйса ведите себя прилично {message.author.mention}')
    if msg in pashal:
        emb = discord.Embed(title= '', colour= 0x770a5b)
        emb.set_image(url= 'https://sun9-9.userapi.com/c856120/v856120067/21d12f/slRVkbnufVg.jpg')
        await message.channel.send(embed= emb)
 
@Bot.command()
async def монетка(ctx):
    await ctx.send('Выбери сторону ' f'{ctx.message.author.mention}:'' орёл или решка')
@Bot.command(pass_context= True)
async def орёл(ctx):
    if monetka == 1:
        await ctx.send('Выпал орёл, ты выиграл ' f'{ctx.message.author.mention}')
    else:
        await ctx.send('Выпала решка, ты проиграл ' f'{ctx.message.author.mention}')
@Bot.command()
async def решка(ctx):
    if monetka == 1:
        await ctx.send('Выпал орёл, ты проиграл ' f'{ctx.message.author.mention}')
    else:
        await ctx.send('Выпала решка, ты выиграл ' f'{ctx.message.author.mention}')
@Bot.command()
async def орел(ctx):
    if monetka == 1:
        await ctx.send('Выпал орёл, ты выиграл ' f'{ctx.message.author.mention}')
    else:
        await ctx.send('Выпала решка, ты проиграл ' f'{ctx.message.author.mention}')

@Bot.command()
async def рулетка(ctx):
    await ctx.send('Ну что готов ' f'{ctx.message.author.mention}, ' 'да?')
@Bot.command()
async def да(ctx):
    if russian_rulette != 1 :
        await ctx.send('Чертов везунчик ' f'{ctx.message.author.mention}')
    if russian_rulette == 1:
        await ctx.send('Допрыгался, ну и ладно ' f'{ctx.message.author.mention}')

@Bot.command()
async def time(ctx):
    now_date = datetime.datetime.now()
    await ctx.send(now_date)

@Bot.command(aliases = ['я', 'карта', 'профиль'])
async def card_user(ctx):
    await ctx.channel.purge(limit = 1)
    img = Image.new('RGBA', (800, 400), '#82378f')
    url = str(ctx.author.avatar_url)[:-10]  # /size=1024   (разрешение)
    response = requests.get(url, stream = True)
    response = Image.open(io.BytesIO(response.content))
    response = response.convert('RGBA')
    response = response.resize((220, 220), Image.ANTIALIAS)
    img.paste(response, (30, 30, 250, 250)) #1-отступ слева 2-отступ сверху 3 и 4 размер (от координат)
    idraw = ImageDraw.Draw(img)
    name = ctx.author.name
    tag = ctx.author.discriminator
    headline = ImageFont.truetype('arial.ttf', size = 40)
    undertext = ImageFont.truetype('arial.ttf', size = 24)
    idraw.text((290, 30), f'{name}#{tag}', font = headline)
    idraw.text((290, 80), f'ID: {ctx.author.id}', font = undertext)
    idraw.text((290, 110), f'Guild: {ctx.author.guild}', font = undertext)
    img.save('user_card.png')
    await ctx.send(file = discord.File(fp = 'user_card.png'))

Bot.run(TOKEN)