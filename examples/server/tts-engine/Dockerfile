# Use an official Node.js runtime as a base
FROM node:18

# Install additional tools (e.g., ffmpeg for audio processing)
RUN apt-get update && apt-get install -y ffmpeg && apt-get clean

# Set the working directory
WORKDIR /usr/src/app

# Copy the package.json and package-lock.json to the container
COPY package*.json ./

# Install the necessary dependencies
RUN npm install

# Copy the rest of the application into the container
COPY . .

# Expose port 3000
EXPOSE 3000

# Start the application
CMD ["npm", "start"]
