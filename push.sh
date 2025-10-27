#!/bin/bash

if [ -z "$1" ]; then
  echo -e "\033[31mError: Commit message required.\033[0m"
  exit 1
fi

# Get current email from git config
CURRENT_EMAIL=$(git config user.email)

# Only sanitize 42wolfsburg emails
if [[ "$CURRENT_EMAIL" == *@*.42wolfsburg.de ]]; then
  SANITIZED_EMAIL=$(echo "$CURRENT_EMAIL" | sed -E 's/^([^@]+)@[^@]+\.42wolfsburg\.de$/\1@student.42wolfsburg.de/')
else
  SANITIZED_EMAIL="$CURRENT_EMAIL"
fi

# Add changes
git add .

# Commit using both author and committer email sanitized
GIT_AUTHOR_EMAIL="$SANITIZED_EMAIL" \
GIT_COMMITTER_EMAIL="$SANITIZED_EMAIL" \
git commit -m "$1"

# Push
git push

echo "Done! Commit pushed with sanitized email: $SANITIZED_EMAIL"
