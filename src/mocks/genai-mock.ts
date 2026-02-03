export class GenAIClientMock {
  generate() {
    console.warn('Using GenAI mock; replace with real client when registry access restored');
    return Promise.resolve({ output: 'mocked result' });
  }
}
