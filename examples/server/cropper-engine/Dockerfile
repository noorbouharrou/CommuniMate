# Gebruik een officiële Node.js runtime als basis
FROM node:18

# Stel de werkdirectory in
WORKDIR /usr/src/app

# Kopieer de package.json en package-lock.json naar de container
COPY package*.json ./

# Installeer de benodigde afhankelijkheden
RUN npm install

# Kopieer de rest van de applicatie naar de container
COPY . .

# Stel de container in om op poort 3000 te luisteren
EXPOSE 3000

# Start de applicatie
CMD ["npm", "start"]
