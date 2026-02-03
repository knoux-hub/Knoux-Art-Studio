export const KnouxSonar = {
  async scanProject(): Promise<{ completionPercentage: number }> {
    return Promise.resolve({ completionPercentage: 100 });
  },
};
