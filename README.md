# QUIZLET 2 ANKI

A simple tool to clone a Quizlet deck provided the link to the deck, extracting terms and definitions for use on better platforms like Anki.

## Features

- Features a C program that is compiled to create an executable that formats the raw data into a formatted text that is saved in a seperate file 
- Features a C program that is compiled to create an executable that creates a server
  - / GET request respons with index.html
  - /submitLink POST responds with an indication whether or not the link was valid
  
## License

This project is released under the MIT License.

# TO DO

- CREATE a loading animation for scraping and transforming quizlet contents
- CREATE a cancel button for the user to return to the main page after the link is validated
- UPDATE the server to scrape the contents of the loaded page via the link entered
- UPDATE the server to return the scraped processed content as a downloadable text file
- UPDATE Q2AServer.c increase the runtime efficiency of  by implementing a trie to handle the routes
- UPDATE Q2ATransformer.c increase the memory efficiency of by reading and processing the contents at once line by line