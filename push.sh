#!/bin/bash

# Ensure a commit message is provided
if [ -z "$1" ]; then
  echo -e "\033[31mError: Commit message is required as the first argument.\033[0m"
  echo "Usage: ./push.sh \"Your commit message\""
  exit 1
fi

# Ensure git is installed
if ! command -v git &> /dev/null; then
  echo "Error: Git is not installed. Please install it and try again."
  exit 1
fi

# Function to clean school email
clean_email() {
  local email=$1
  # Replace numbers or machine IDs before @ with 'student', leave domain intact
  echo "$email" | sed -E 's/^([^@]+)@[^@]+\.42wolfsburg\.de$/\1@student.42wolfsburg.de/'
}

# Get current committer from Git config (this is where your cluster email is)
CURRENT_COMMITTER_EMAIL=$(git config user.email)
echo "Current committer email: $CURRENT_COMMITTER_EMAIL"

# Only sanitize if it is a 42wolfsburg email
if [[ "$CURRENT_COMMITTER_EMAIL" == *@*.42wolfsburg.de ]]; then
  NEW_COMMITTER_EMAIL=$(clean_email "$CURRENT_COMMITTER_EMAIL")
else
  NEW_COMMITTER_EMAIL="$CURRENT_COMMITTER_EMAIL"
fi

# Add changes
echo "Adding changes..."
git add .

# Commit using the dynamic committer email
echo "Committing with committer email: $NEW_COMMITTER_EMAIL"
GIT_COMMITTER_EMAIL="$NEW_COMMITTER_EMAIL" git commit -m "$1"

# Push changes
echo "Pushing changes..."
git push

echo "Done! Commit pushed with committer email: $NEW_COMMITTER_EMAIL"
