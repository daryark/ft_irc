#!/bin/bash

# Set the old email pattern to match and the new email to replace it with
OLD_EMAIL_PATTERN=".*42wolfsburg.de"
NEW_AUTHOR_NAME="Daria Yarkovska"  # Change this to your desired name
NEW_AUTHOR_EMAIL="dyarkovs@student.42wolfsburg.de"

# Ensure git is installed
if ! command -v git &> /dev/null; then
  echo "Error: Git is not installed. Please install it and try again."
  exit 1
fi

# Rewriting commit history
echo "Rewriting commit history to change author emails..."
git filter-repo --quiet --force --commit-callback '
import re

if re.match(r'"'"$OLD_EMAIL_PATTERN"'"', commit.original_author_email):
    commit.author_name = "'"$NEW_AUTHOR_NAME"'"
    commit.author_email = "'"$NEW_AUTHOR_EMAIL"'"
'

if [ $? -eq 0 ]; then
  echo "Successfully updated author emails for matching commits."
else
  echo "Error: Failed to rewrite author emails."
  exit 1
fi

echo "Done!"
