module.exports = {
    "roots": [
      "src"
    ],
    "testMatch": [
      "**/__tests__/src/**/*.+(ts|tsx|js)",
      "**/?(*.)+(spec|test).+(ts|tsx|js)"
    ],
    "transform": {
      "^.+\\.(ts|tsx)$": "ts-jest"
    },
  }
